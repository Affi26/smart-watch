import sqlite3

import hike
import threading
from datetime import datetime

DB_FILE_NAME = 'sessions.db'

DB_SESSION_TABLE = {
    "name": "sessions",
    "cols": [
        "session_id integer PRIMARY KEY",
        "m integer",
        "steps integer",
        "burnt_kcal integer",
        "duration integer",
        "session_time text"
    ]
}

#"duration integer",
#"session_time text"

# lock object so multithreaded use of the same
# HubDatabase object 

class HubDatabase:
    """Hiking sesssion database interface class.

    An object of this class enables easy retreival and management of the
    hiking database content. If the database does not exist, the instantiation
    of this class will create the database inside `DB_FILE_NAME` file.
    
    Arguments:
        lock: lock object so multithreaded use of the same HubDatabase object
              is safe. sqlite3 does not allow the same cursor object to be
              used concurrently.
        con: sqlite3 connection object
        cur: sqlite3 cursor object
    """

    lock = threading.Lock()

    def __init__(self):
        self.con = sqlite3.connect(DB_FILE_NAME, check_same_thread=False)
        self.cur = self.con.cursor()

        for t in [DB_SESSION_TABLE]:
            create_table_sql = f"create table if not exists {t['name']} ({', '.join(t['cols'])})"
            self.cur.execute(create_table_sql)

        create_goals_table_sql = f"create table if not exists {DB_SESSION_TABLE['name']} ({', '.join(DB_SESSION_TABLE['cols'])})"
        self.cur.execute(create_goals_table_sql)

        self.con.commit()

    def save(self, s: hike.HikeSession):
        sessions = self.get_sessions()

        if len(sessions) > 0:
            s.id = sorted(sessions, key=lambda sess: sess.id)[-1].id + 1
        else:
            s.id = 1

        try:
            self.lock.acquire()

            try:
                #print(f"Values to insert: m = {s.m}, steps = {s.steps}, kcal= {s.kcal}")
                #self.cur.execute(f"INSERT INTO {DB_SESSION_TABLE['name']} VALUES ({s.id}, {s.distance}, {s.steps}, {s.kcal})")
                self.cur.execute(f"INSERT INTO {DB_SESSION_TABLE['name']} VALUES ({s.id}, {s.distance}, {s.steps}, {s.kcal}, {s.duration}, {s.session_time})")
                #self.cur.execute(f"INSERT INTO {DB_SESSION_TABLE['name']} VALUES ({s.id}, {s.distance}, {s.steps}, {s.kcal}, {s.session_time})")

            except sqlite3.IntegrityError:
                print("WARNING: Session ID already exists in database! Aborting saving current session.")


            self.con.commit()
        finally:
            self.lock.release()

    def delete(self, session_id: int):
        try:
            self.lock.acquire()

            self.cur.execute(f"DELETE FROM {DB_SESSION_TABLE['name']} WHERE session_id = {session_id}")
            self.con.commit()
        finally:
            self.lock.release()

    def get_sessions(self) -> list[hike.HikeSession]:
        try:
            self.lock.acquire()
            rows = self.cur.execute(f"SELECT * FROM {DB_SESSION_TABLE['name']}").fetchall()
        finally:
            self.lock.release()

        #return [hike.from_list([row[0], row[1], row[2], row[3], datetime.fromisoformat(row[4])]) for row in rows]
        return list(map(lambda r: hike.from_list(r), rows))

    def get_session(self, session_id: int) -> hike.HikeSession:
        try:
            self.lock.acquire()
            rows = self.cur.execute(f"SELECT * FROM {DB_SESSION_TABLE['name']} WHERE session_id = {session_id}").fetchall()
        finally:
            self.lock.release()

        return hike.from_list(rows[0])

    def __del__(self):
        self.cur.close()
        self.con.close()

    def save_goals(self, steps_goal: int, calories_goal: int):
        try:
            self.lock.acquire()

            # Create the goals table if it doesn't exist
            create_goals_table_sql = f"CREATE TABLE IF NOT EXISTS goals (steps_goal INTEGER, calories_goal INTEGER)"
            self.cur.execute(create_goals_table_sql)

            # Insert or replace goals data into the goals table
            self.cur.execute(f"INSERT OR REPLACE INTO goals (steps_goal, calories_goal) VALUES (?, ?)", (steps_goal, calories_goal))

            self.con.commit()
        finally:
            self.lock.release()

    def get_goals(self):
        try:
            self.lock.acquire()

            # Create the goals table if it doesn't exist
            create_goals_table_sql = f"CREATE TABLE IF NOT EXISTS goals (steps_goal INTEGER, calories_goal INTEGER)"
            self.cur.execute(create_goals_table_sql)

            # Fetch goals data from the goals table
            rows = self.cur.execute("SELECT * FROM goals").fetchall()
        finally:
            self.lock.release()

        # If goals table is empty, return None
        if not rows:
            return None

        # Return the goals data
        return {'steps_goal': rows[0][0], 'calories_goal': rows[0][1]}
    
    def clear_previous_goals(self):
        try:
            self.lock.acquire()

            # Execute SQL query to delete all rows from the goals table
            self.cur.execute("DELETE FROM goals")
            
            self.con.commit()
        finally:
            self.lock.release()

