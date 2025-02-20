#include <stdio.h>
#include <Windows.h>

#define IMAGE_OPT_HEADER_MAGIC_PE32 0x10b
#define IMAGE_OPT_HEADER_MAGIC_PE64 0x20b

void print_os(WORD Major, WORD minor) {
	switch (Major)
	{
	case 3:
		printf("Windows NT 3.1+\n");
		break;
	case 4:
		printf("Windows 95+\n");
		break;
	case 5:
		printf("Windows 2000+\n");
		break;
	case 6:
		switch (minor)
		{
		case 0: printf("Windows Vista+\n"); break;
		case 1: printf("Windows 7+\n"); break;
		case 2: printf("Windows 8+\n"); break;
		case 3: printf("Windows 8.1+\n"); break;
			break;
		default: printf("Unknown Windows 6.x version\n");
			break;
		}
		break;
	case 10:
		printf("Windows 10+\n");
		break;
	default:
		printf("Unknown OS version\n");
		break;
	}
}



// https://learn.microsoft.com/en-us/windows/win32/debug/pe-format#windows-subsystem
// Checks the Meaning the value of OptionalHeader->SubSystem Means
void print_subSystem(WORD SubSystem) {
	switch (SubSystem) {
	case 1:
		printf("|---SUBSYSTEM: %X | Native\n", SubSystem);
		break;
	case 2:
		printf("|---SUBSYSTEM: %X | Windows GUI\n", SubSystem);
		break;
	case 3:
		printf("|---SUBSYSTEM: %X | Windows Console\n", SubSystem);
		break;
	case 5:
		printf("|---SUBSYSTEM: %X | Windows CE\n", SubSystem);
		break;
	case 7:
		printf("|---SUBSYSTEM: %X | EFI Application\n", SubSystem);
		break;
	case 8:
		printf("|---SUBSYSTEM: %X | Windows Boot Application\n", SubSystem);
		break;
	case 9:
		printf("|---SUBSYSTEM: %X | MS-DOS\n", SubSystem);
		break;
	case 10:
		printf("|---SUBSYSTEM: %X | Windows Console\n", SubSystem);
		break;
	case 11:
		printf("|---SUBSYSTEM: %X | Windows GUI (Windows 2000+)\n", SubSystem);
		break;
	case 12:
		printf("|---SUBSYSTEM: %X | Windows Native (64-bit)\n", SubSystem);
		break;
	case 13:
		printf("|---SUBSYSTEM: %X | Windows CE 5.0\n", SubSystem);
		break;
	case 15:
		printf("|---SUBSYSTEM: %X | Windows Longhorn\n", SubSystem);
		break;
	default:
		printf("|---SUBSYSTEM: %X | Unknown\n", SubSystem);
		break;
	}
}

int main(int argc, char *argv[]) {
	const char fileName[255];
	PIMAGE_SECTION_HEADER SectionHeader;
	PIMAGE_SECTION_HEADER importSection = { 0 };
	PIMAGE_IMPORT_DESCRIPTOR importDescriptor;
	PIMAGE_THUNK_DATA thunkData = { 0 };

	DWORD thunk;
	memcpy(fileName, argv[1], 255);

	HANDLE hFile = NULL;
	if (!(hFile = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL))) {
		printf("Failed Opening File: %d\n", GetLastError());
		return 0;
	}
	
	DWORD FileSize = NULL;
	if (!(FileSize = GetFileSize(hFile, NULL))) {
		printf("Failed Getting Size: %d\n", GetLastError());
		return 0;
	}
	printf("FILE SIZE: %d\n", FileSize);

	BYTE* Buffer = (BYTE*)malloc(FileSize);
	DWORD BytesRead = NULL;

	ReadFile(hFile, Buffer, FileSize, &BytesRead, NULL);

	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)Buffer;
	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
		printf("[!] WRONG MAGIC BYTES\n[!] CHOOSE A VALID FILE\n");
	}

	

	printf("==================== DOS HEADER ====================\n");
	printf("|---MAGIC BYTES: 0x%02x\n", pDosHeader->e_magic);
	printf("|---BYTES ON LAST PAGE OF FILE: %hu\n", (WORD)pDosHeader->e_cblp);
	printf("|---PAGES IN FILE: %hu\n", " ", pDosHeader->e_cp);
	printf("|---RELOCATIONS: %hu\n", " ", pDosHeader->e_crlc);
	printf("|---SIZE OF HEADER: %hu\n", pDosHeader->e_cparhdr);
	printf("|---MININUM EXTRA PARGRAPHS NEEDED: %hu\n", pDosHeader->e_minalloc);
	printf("|---MAXIMUM EXTRA PARAGRAPHS NEEDED: 0x%04x\n", pDosHeader->e_maxalloc);
	printf("|---INITAL SS VALUE: %hu\n", pDosHeader->e_ss);
	printf("|---INITIAL SP VALUE: 0x%02x\n", pDosHeader->e_sp);
	printf("|---CHECKSUM: %hu\n", pDosHeader->e_csum);
	printf("|---INTIAL IP VALUE: %hu\n", pDosHeader->e_ip);
	printf("|---INTIAL CS VALUE: %hu\n", pDosHeader->e_cs);
	printf("|---FILE ADDRESS OF RELOCATION: %02x\n", pDosHeader->e_lfarlc);
	printf("|---OVERLAY NUMBER: %hu\n", pDosHeader->e_ovno);
	printf("|---RESERVED: %hu,%hu,%hu,%hu\n", pDosHeader->e_res[0], pDosHeader->e_res[1], pDosHeader->e_res[2], pDosHeader->e_res[3]);
	printf("|---OEM IDENTIFIER: %hu\n", pDosHeader->e_oemid);
	printf("|---OEM INFORMATION: %hu\n", pDosHeader->e_oeminfo);
	printf("|---RESERVED: ");
	for (int i = 0; i < 10; i++) {
		if (i == 9) {
			printf("%hu\n", pDosHeader->e_res2[9]);
			break;
		}
		printf("%hu, ", pDosHeader->e_res2[i]);
	}
	printf("|---FILE ADDRESS OF NT HEADER: 0x%02x\n\n", pDosHeader->e_lfanew);
	printf("+=================== NT HEADER ====================+\n");

	PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(Buffer + pDosHeader->e_lfanew);
	printf("|----SIGNATURE: %X\n", pNtHeader->Signature);
	printf("|----FILE HEADERS\n");
	printf("\t|---MACHINE: %04x (%s)\n", pNtHeader->FileHeader.Machine, pNtHeader->FileHeader.Machine == 0x8664 ? "AMD64" : "i386");
	printf("\t|---NUMBER OF SECTIONS: %hu\n", pNtHeader->FileHeader.NumberOfSections);
	printf("\t|---TIME DATE STAMP: %04x\n", pNtHeader->FileHeader.TimeDateStamp);
	printf("\t|---PTR SYMBOL TABLES: %X\n", pNtHeader->FileHeader.PointerToSymbolTable);
	printf("\t|---NUMBER OF SYMBOLS: %X\n", pNtHeader->FileHeader.NumberOfSymbols);
	printf("\t|---OPTION HEADER SIZE: %X | %d\n", pNtHeader->FileHeader.SizeOfOptionalHeader, (INT)pNtHeader->FileHeader.SizeOfOptionalHeader);
	printf("\t|---CHARACTERISTICS: %X\n", pNtHeader->FileHeader.Characteristics);
	printf("\n==================== OPT HEADER =====================\n");
	PIMAGE_OPTIONAL_HEADER pOptHeader = (PIMAGE_OPTIONAL_HEADER)&pNtHeader->OptionalHeader;
	printf("|---MAGIC BYTES: %X (%s) \n", pOptHeader->Magic, pOptHeader->Magic == IMAGE_OPT_HEADER_MAGIC_PE64 ? "PE32+" : "PE32");
	printf("|---MAJOR LINK VERSION: %X\n", pOptHeader->MajorLinkerVersion);
	printf("|---MINOR LINK VERSION: %X\n", pOptHeader->MinorLinkerVersion);
	printf("|---SIZE OF CODE: %X | %lu\n", pOptHeader->SizeOfCode, pOptHeader->SizeOfCode);
	printf("|---SIZE OF INIT DATA: %X | %lu\n", pOptHeader->SizeOfInitializedData, pOptHeader->SizeOfInitializedData);
	printf("|---SIZE OF UNINIT DATA: %X | %lu\n", pOptHeader->SizeOfUninitializedData, pOptHeader->SizeOfUninitializedData);
	printf("|---SIZE OF IMAGE: %X | %lu\n", pOptHeader->SizeOfImage, pOptHeader->SizeOfImage);
	printf("|---SIZE OF HEADERS: %X | %lu\n", pOptHeader->SizeOfHeaders, pOptHeader->SizeOfHeaders);
	printf("|---ADDRESS ENTRY POINT: %X\n", pOptHeader->AddressOfEntryPoint);
	printf("|---BASE OF CODE: %X\n", pOptHeader->BaseOfCode);
	printf("|---IMAGE BASE: %X\n", pOptHeader->ImageBase);
	printf("|---SECTION ALIGNMENT: %X\n", pOptHeader->SectionAlignment);
	printf("|---FILE ALIGNMENT: %X\n", pOptHeader->FileAlignment);
	printf("|---MAJOR OS VERSION: %X | ", pOptHeader->MajorOperatingSystemVersion);
	print_os(pOptHeader->MajorOperatingSystemVersion, pOptHeader->MinorOperatingSystemVersion);
	printf("|---MINOR OS VERSION: %X\n", pOptHeader->MinorOperatingSystemVersion);
	printf("|---MAJOR IMAGE VERSION: %X\n", pOptHeader->MajorImageVersion);
	printf("|---MINOR IMAGE VERSION: %X\n", pOptHeader->MinorImageVersion);
	printf("|---MAJOR SUBSYSTEM VERSION: %X\n", pOptHeader->MajorSubsystemVersion);
	printf("|---WIN32 VERSION: %X\n", pOptHeader->Win32VersionValue);
	printf("|---CHECKSUM: %X\n", pOptHeader->CheckSum);
	print_subSystem(pOptHeader->Subsystem);
	printf("|---DLL CHARACTERISTICS: %X\n", pOptHeader->DllCharacteristics);

	// Check the charastistics using the bitwise XOR
	// https://learn.microsoft.com/en-us/windows/win32/debug/pe-format#windows-subsystem
	if (pOptHeader->DllCharacteristics & 0x0020) printf("\t|---High Entropy ASLR\n");
	if (pOptHeader->DllCharacteristics & 0x0040) printf("\t|---Base Can be Realocated Using ASLR\n");
	if (pOptHeader->DllCharacteristics & 0x0080) printf("\t|---Code Integrity Checks Are Enforced\n");
	if (pOptHeader->DllCharacteristics & 0x0100) printf("\t|---Image is NX Capable\n");
	if (pOptHeader->DllCharacteristics & 0x0200) printf("\t|---Isolation Aware, Do Not Isolate\n");
	if (pOptHeader->DllCharacteristics & 0x0400) printf("\t|---Does Not Use SE\n");
	if (pOptHeader->DllCharacteristics & 0x0800) printf("\t|---Do Not Bind the Image\n");
	if (pOptHeader->DllCharacteristics & 0x1000) printf("\t|---Image Must Execute in AppContainer\n");
	if (pOptHeader->DllCharacteristics & 0x2000) printf("\t|---A WDM Driver\n");
	if (pOptHeader->DllCharacteristics & 0x4000) printf("\t|---Image Supports Control Flow Gaurd\n");
	if (pOptHeader->DllCharacteristics & 0x8000) printf("\t|---Terminal Server Aware\n");

	printf("|---SIZE OF STACK RESERVE: %X\n", pOptHeader->SizeOfStackReserve);
	printf("|---SIZE OF STACK COMMIT: %X\n", pOptHeader->SizeOfStackCommit);
	printf("|---SIZE OF HEAP RESERVE: %X\n", pOptHeader->SizeOfHeapReserve);
	printf("|---SIZE OF HEAP COMMIT: %X\n", pOptHeader->SizeOfHeapCommit);
	printf("|---LOADER FLAGS: %X\n", pOptHeader->LoaderFlags);
	printf("|---NUMBER OF RVA/SIZES: %X\n", pOptHeader->NumberOfRvaAndSizes);

	printf("+=================== DATA DIRECTORY ====================+\n");
	printf("|---EXPORT DIRECTORY ADDRESS: %X\n", pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	printf("|---EXPORT DIRECTORY SIZE: %X\n", pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size);
	printf("|---IMPORT DIRECTORY ADDRESS: %X\n", pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
	printf("|---IMPORT DIRECTORY SIZE: %X\n\n", pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size);
	
	printf("+=================== SECTION HEADERS ====================+\n");

	SectionHeader = IMAGE_FIRST_SECTION(pNtHeader);
	DWORD ImportDirectoryRVA = pOptHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;

	for (int i = 0; i < pNtHeader->FileHeader.NumberOfSections; i++, SectionHeader++) {
		printf("|---%s\n", SectionHeader->Name);
		printf("\t|---VIRTUAL SIZE: %X\n", SectionHeader->Misc.VirtualSize);
		printf("\t|---VIRTUAL ADDRESS: %X\n", SectionHeader->VirtualAddress);
		printf("\t|---RAW DATA SIZE: %X\n", SectionHeader->SizeOfRawData);
		printf("\t|---POINTER TO RAW DATA: %X\n", SectionHeader->PointerToRawData);
		printf("\t|---POINTER TO LINE NUMBERS: %X\n", SectionHeader->PointerToLinenumbers);
		printf("\t|---POINTER TO REALOCATIONS: %X\n", SectionHeader->PointerToRelocations);
		printf("\t|---NUMBER OF LINE NUMBERS: %X\n", SectionHeader->NumberOfLinenumbers);
		printf("\t|---CHARACTERISTICS: %X\n", SectionHeader->Characteristics);

		if (ImportDirectoryRVA >= SectionHeader->VirtualAddress &&
			ImportDirectoryRVA < SectionHeader->VirtualAddress + SectionHeader->Misc.VirtualSize) {
			importSection = SectionHeader;
		}
	}
	printf("\n+=================== DLL IMPORTS ====================+\n");
	importDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)((ULONG_PTR)Buffer + (ImportDirectoryRVA - importSection->VirtualAddress + importSection->PointerToRawData));

	BYTE* RawOffset = (BYTE*)Buffer + importSection->PointerToRawData;

	while (importDescriptor->Name) {
		printf("|---%s\n", RawOffset + (importDescriptor->Name - importSection->VirtualAddress));

		thunk = importDescriptor->OriginalFirstThunk;

		thunkData = (PIMAGE_THUNK_DATA)((ULONG_PTR)RawOffset + (thunk - importSection->VirtualAddress));

		while (thunkData->u1.AddressOfData != 0) {
			if (thunkData->u1.AddressOfData < 0x80000000) {
				printf("\t%s\n", RawOffset + (thunkData->u1.Function - importSection->VirtualAddress + 2));
				thunkData++;
			}
		}
		importDescriptor++;
	}
	

}
