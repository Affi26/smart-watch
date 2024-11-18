from flask import Flask, render_template, jsonify, Response, redirect, request, url_for

import db
import hike

app = Flask(__name__)
hdb = db.HubDatabase()

#def read_html_file(filename):
#    return open(filename, 'r').read()

@app.route('/')
def get_home():
    sessions = hdb.get_sessions() 
    sessions_data = [{'id': session.id, 'distance': session.distance, 'steps': session.steps, 'kcal': session.kcal, 'session_time': session.session_time, 'duration': session.duration} for session in sessions]
    return render_template('home.html', sessions=sessions, sessions_data=sessions_data)

@app.route('/sessions')
def get_sessions():
    sessions = hdb.get_sessions() 
    sessions_data = [{'id': session.id, 'distance': session.distance, 'steps': session.steps, 'kcal': session.kcal, 'session_time': session.session_time, 'duration': session.duration} for session in sessions]
    return render_template('sessions.html', sessions=sessions, sessions_data=sessions_data)

@app.route('/sessions/<id>')
def session_details(id):
    session = hdb.get_session(id)
    if session:
        session_data = {
            'id': session.id,
            'distance': session.distance,
            'steps': session.steps,
            'kcal': session.kcal,
            'session_time': session.session_time,
            'duration': session.duration
        }
        
        # Retrieve the latest goals data from the database
        goals_data = hdb.get_goals()

        return render_template('session_details.html', session=session_data, goals_data=goals_data)
    else:
        return "Session not found.", 404

@app.route('/sessions/<id>/delete')
def delete_session(id):
    hdb.delete(id)
    print(f'DELETED SESSION WITH ID: {id}')
    return redirect('/sessions')

@app.route('/goals', methods=['GET', 'POST'])
def goals():
    if request.method == 'POST':
        steps_goal = int(request.form['steps_goal'])
        calories_goal = int(request.form['calories_goal'])
        
        # Clear previous data
        hdb.clear_previous_goals()

        # Save the goals to the database
        hdb.save_goals(steps_goal, calories_goal)

    # Retrieve the latest goals data from the database
    goals_data = hdb.get_goals()
    
    print(goals_data)

    return render_template('goals.html', current_goals=goals_data)



if __name__ == "__main__":
    app.run('0.0.0.0', debug=True)
