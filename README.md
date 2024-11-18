# Smart Watch


## Description

Activity Tracker is a web-based application designed to help users monitor and track their physical activities, such as walking or running. It allows users to set goals for steps and calories, view detailed information about their activities, and visualize their progress over time.

## Features

- Set Goals: Users can set daily targets for steps and calories.
- Track Activities: Detailed tracking of steps, distance covered, and calories burned.
- Session Management: View, add, and delete activity sessions.
- Data Visualization: Interactive charts to visualize progress.
- Responsive Design: Accessible on various devices with a clean, user-friendly interface.

## Technology Stack

- Frontend: HTML, CSS, JavaScript
- Charting Library: Chart.js for data visualization
- Backend: Flask
- Database: SQL

## Getting Started
## Prerequisites
For RaspberryPi:  
pybluez==0.23  
flask==1.1.2  
pillow==8.1.2  
unicornhathd=0.0.4 you need to install this as sudo  

## Installation  

**1. Clone the repository**
`git clone git@version.aalto.fi:embedded-project-group-k/smart-watch.git`

**2. Navigate to the project directory. Project contains two main directories**    

RPi/  
│    
├── templates/ - Folder with HTML files, it contains all wev pages for the server.  
├── bt.py - Bluetooth interface module for handling connections and data synchronization.  
├── db.py - Database module responsible for all database-related operations.  
├── hike.py - Core module defining the HikeSession class and related functionalities.    
├── receiver.py - Server-side module for receiving and processing data from the watch.  
├── requirements.txt - Specifies all the Python dependencies for the project.  
├── sessions.db - SQLite database file containing the activity sessions data.  
└── wserver.py - Web server module that serves the application's frontend and API.  
  
TTGo_watch_fw/  
│  
├── .vscode/              - VSCode specific settings and configurations.  
├── include/              - Header files for shared function declarations.  
├── lib/                  - Libraries used in the project.  
├── misc/                 - Miscellaneous files related to the project.  
├── src/                  - Source files of the smart watch firmware.  
│   ├── config.h          - Configuration header file.  
│   ├── main.cpp          - Main entry point for the smart watch firmware.  
│   ├── utils.cpp         - Utility functions implementation.  
│   └── utils.h           - Header for utility function declarations.  
├── test/                 - Test scripts and testing-related files.  
├── .gitignore            - Specifies intentionally untracked files to ignore.  
└── platformio.ini        - PlatformIO configuration file.  
  
**3. Installing the project**  
### Watch:   
    1. Installing VSCode for debugging the Watch software.  
        https://code.visualstudio.com/download  
    2. Also we will use the PlatformIO for uploading the code to watches  
        https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide  
    3. After downloading PlatformIO we need to:   
        1. Restart VSCode  
        2. Click the PlatformIO icon on the left toolbar   
        3. Open the project (TTGo_watch_fw)  
        4. Go to src/main.cpp  
        5. Compile it with the button in the bottom toolbar (looks like the check mark)  
        6. After successfull download, upload the project to the watch, having previously connected them via USB  

### Rpi:   
    1. Download repository.   
    2. Make sure all prerequisite packages are installed (see above).
    3. The default code editor provided with the Raspberry Pi is sufficient to debug the python scripts.
    4. If all the steps are followed correctly, the receiver and webserver scripts should run without issues.

## Running the application  
### Watch  
Once the watch is powered on. It will automatically come to the Hiking App interface. To initiate the hiking session, press the power button once. Then start your adventure, the app will keep on track of it. 

After your hiking adventure, you can save your hiking session data to your docking station.   

First make sure the watch is connected to the docking station through Bluetooth. (BLE).  
  
Then simply press the power button to end your stop your session. Then confirm the data transfer before the countdown ends. If the button is not pressed again within the 5 seconds, step counting will continue. This will prevent accidental session stops or data losses.  
![Instruction for wathces](/Watch_instruction.png "Instruction")   

### RPi  
**Setup Bluetooth Connection**   

First connect the watch to Raspberry pi using Bluetooth. This is a one-time process.  

![Instruction for bluetooth](/Bluetooth_connection.png "Bluetooth")  

**Transfer data**  
Run the receiver.py with  
`Python 3 receiver.py`
After you stopped your session, you need place the watches near the RPi, it will automatically connect and tracnsfer the data to the Rpi. In case successfull transfer, you will see a recorded parametrs from the Watch. 

**Start the Server**  
Run the wserver.py with   
`Python 3 wserver.py`  
In command prompt you will see further instructions. Go to localhost addres with your server (by default: 127.0.0.1:5000). There you will see a home menu.  
First press ‘Goals’ to set your step and distance goals.
Press ‘Sessions’ to view the past recorded session data.
If you want to remove a particular session data from the Activity Tracker, press ‘Delete’ in front of it.  
To see the detailed analysis of a session. Press the ‘ID number’ of the corresponding session.   
   
## Description of architecture  

The system is designed with a modular approach, dividing responsibilities among different components written in Python and C++.
    
At the user interface level, there's a web page that serves as the point of interaction for the user. With `wserver.py` starting the local web server to render the web pages. This web page is dynamically served by a Flask server.    
On the server side, `db.py` outlines the data handling layer, describing how the data is stored in the database and defining functions for operations like deletion. This script interacts with `sessions.db`, a database file that stores session data across various columns like session ID, steps, burnt kcal, duration, and session time, providing a structured approach to storing workout data. 

The Bluetooth communication is managed by `bt.py`, which sets up the Bluetooth connection with the receiver script `reciever.py`. This receiver script is responsible for handling incoming data from the Bluetooth connection. `main.cpp` appears to handle raw fitness data, including IDs, steps, distances, and session times, received from the fitness tracker and stored in text files like `id.txt`, `steps.txt`, `distance.txt`, and `sessiontime.txt`. 

`hike.py` is another Python script that seems to deal with the business logic, handling variables specific to hiking activities, and it may also work in conjunction with reciever.py to process the incoming data before it is stored in the database.  

![Architecture](/Architecture.png "Architecture")  
![software architecture](/Code_architecture.png "Architecture") 

## Error Handling and Troubleshooting  

There are some potential problems which you can encounter: 

**Bluetooth Connectivity Issues:**  
    - Problem: Difficulty in establishing or maintaining a Bluetooth connection between the watch and the Raspberry Pi.  
    - Solution: Ensure that both devices are within range, restart both devices to reset Bluetooth configurations, and verify that no other Bluetooth devices are interfering with the connection. Updating Bluetooth drivers on the Raspberry Pi might also help.   

**Web Server Issues:**  
    - Problem: The Flask web server not starting or not accessible.  
    - Solution: Verify that Flask is correctly installed and that all dependencies are met. Check the command-line output for any error messages. Ensure that the server port is not being blocked by a firewall or already in use by another application.  
 
**Installation and Dependency Issues:**  
    - Problem: Errors during installation due to missing or conflicting dependencies.  
    - Solution: Follow the installation instructions carefully and ensure all prerequisites are installed. Using a virtual environment for Python projects can help manage dependencies more effectively.      

**Compilation Errors on the Watch Firmware:**  
    - Problem: Issues compiling the firmware for the smart watch.  
    - Solution: Verify that the correct version of PlatformIO is installed. Check for any missing libraries or syntax errors in the code. Make sure that the development environment is set up for the specific hardware of the smart watch.  


## Customization 

Once of the possible Customizations of the project can be adding a new parameter to store from watch. Potentially the User can add many other options, so further you can find a clear step-by-step tutorial how the duration variable was added to this project. Following this update, the user can analize the code and make some simillar steps to customize the project. 

### Watch

1. Let's create a new variable  

`uint32_t steps = 0;`  
`float dist = 0;`   
`unsigned long sessionStartTime = 0; // New variable`  <--------  
`uint32_t elapsedTime = 0;`  

2. configure `void sendSessionBT()`  

`// Sending session id`  
`sendDataBT(LITTLEFS, "/id.txt");`  
`SerialBT.write(';');`  
`// Sending steps`  
`sendDataBT(LITTLEFS, "/steps.txt");`  
`SerialBT.write(';');`  
`// Sending distance`  
`sendDataBT(LITTLEFS, "/distance.txt");`  
`SerialBT.write(';');`  
`// Sending session time`  
`sendDataBT(LITTLEFS, "/sessiontime.txt");`        <---------------  
`SerialBT.write(';');`  
`// Send connection termination char`  
`SerialBT.write('\n');`  

3. Add new function 

`void saveSessiontimeToFile(uint32_t elapsedTime)`  
`{`  
`    char buffer[10];`   
`    itoa(elapsedTime, buffer, 10);`  
`    writeFile(LITTLEFS, "/sessiontime.txt", buffer);`  
`}`  

4. Adjust delete function    
  
`void deleteSession()`    
`{`  
`    deleteFile(LITTLEFS, "/id.txt");`  
`    deleteFile(LITTLEFS, "/distance.txt");`   
`    deleteFile(LITTLEFS, "/steps.txt");`  
`    //deleteFile(LITTLEFS, "/coord.txt");`  
`    deleteFile(LITTLEFS, "/sessiontime.txt");`        <-------------  
`}`    

5. Add to void loop the function `millis()` which returns the number of milliseconds passed since the system began running the current program.  

`case 2:`   
`    {`   
`        /* Hiking session initalisation */`  
`        sensor->begin();`  
`        sessionStartTime = millis();`                <-------------  
`        state = 3;`  
`        break;`  
`    }`  

6. Now by substracting from current time `millis()` the start time `sessionStartTime` we get the duration time.   

`309  elapsedTime = (millis() - sessionStartTime) / 1000; // Convert milliseconds to seconds`   

7. Save `elapsedTime` to `sessiontime.txt`  

`case 4:`   
`    {`   
`        //Save hiking session data`  
`        saveIdToFile(sessionId);`  
`        saveStepsToFile(steps);`  
`        saveDistanceToFile(dist);`    
`        saveSessiontimeToFile(elapsedTime);`             <----------------------  
`        sessionStored = true;`    
`        watch->tft->fillRect(0, 0, 240, 240, TFT_BLACK);`   
`        watch->tft->drawString("Session stored",  45, 100);`   
`        delay(1000);`  
`        state = 1; `  
`        break;`   
`    }`  

This is almost comlete instructions unless it doesnt include the parts with displaying the numbers which you can explore by yourself :)

### RPi 

1. Let's setup the `hike.py`  

`class HikeSession:`  
`   id = 0`  
`   distance = 0`  
`   steps = 0`  
`   kcal = -1`  
`   session_time = datetime.now().strftime("%y%m%d%H%M") #returns string`  
`   duration = 0`                     <------------------     
`   coords = []`    

Also you need to add to ruturn to __repr__ methos the `{self.duration}(sec)`  
to `to_list` the `s.duration`  
to `from_list` the  `s.duration = l[4]`  

2. Add a configuration to `mtos` function in  `bt.py`  

`hs = hike.HikeSession()`  
`        hs.id     = int(parts[0])`  
`        hs.steps  = int(parts[1])`  
`        hs.distance     = float(parts[2])`  
`        hs.duration     = int(parts[3])`  <----------  

3. In `db.py` you need to configure 
  
`DB_SESSION_TABLE = {`  
`    "name": "sessions",`  
`    "cols": [`  
`        "session_id integer PRIMARY KEY",`  
`        "m integer",`  
`        "steps integer",`  
`        "burnt_kcal integer",`  
`        "duration integer",`             <------------  
`        "session_time text"`  
`    ]`  
`}`  

in `save` method insert `{s.duration}` in `execute`  

there is no need to adjust something there more, however, in case you want to add specific local variables, you can add here some methods for taking them from database or specify any operation with them.  

As long the server can work only localy, after turning it off all data will vanish, so in case you want to store some long-term variable (e.g. goals) you will need to implement them into database.  

4. in `wserver.py` you need to add the new variable to all `sessions_data` in `get_home`, `get_session`, `session_details`.    

In case you want to configure the server (e.g. adding new web page), you can declare new functions here also. For instance in `get_sessions()` (the web page with more detailed information about each session) you can see:   

`def get_sessions():`  
`    sessions = hdb.get_sessions() `  
`    sessions_data = [{'id': session.id, 'distance': session.distance, 'steps': session.steps, 'kcal': session.kcal, 'session_time': session.session_time, 'duration': session.duration} for session in sessions]` #data which will be send to the server  
`    return render_template('sessions.html', sessions=sessions, sessions_data=sessions_data)`  # starting the web page `sessions.html`   




 
