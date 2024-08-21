from flask import Flask, render_template, request, redirect, url_for, session

app = Flask(__name__)
app.secret_key = 'supersecretkey'  # Necessary for flash messages

username = "admin"
password = "admin"
output = ""
command = ""
name = ""
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


@app.route("/hostname", methods=['POST', 'GET'])
def hostname():
    if request.method == "POST":
        global name
        name = request.form.get('hostname')
        return '', 200
    else:
        return name



@app.route("/input", methods=['GET', 'POST'])
def input():
    out = output
    names = name
    return render_template("input.html")

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
        return "Nope"

@app.route("/send_command", methods=['POST', 'GET'])
def send_command():
    if request.method == "POST":
        global command
        command = ""
        global output
        output = request.form.get('output') # Store the output in the session
       
        return '', 204  # Return no content to avoid redirection or message
    else:
        return output
        
        
if __name__ == '__main__':
    app.run(host="0.0.0.0", port=80, debug=True)
