from flask import Flask, render_template, request, jsonify
from flask_socketio import SocketIO
from collections import deque
import time
from dotenv import load_dotenv
import openai
import os
load_dotenv()
openai.apikey = os.getenv("OPENAIAPIKEY")

# Flask and SocketIO setup
app = Flask(__name__)
socketio = SocketIO(app)


# Data storage for the last N records
MAX_RECORDS = 100
data_store = deque(maxlen=MAX_RECORDS)

@app.route('/')
def index():
    # Render the front-end page
    return render_template('index.html')

@app.route('/update', methods=['GET'])
def update_data():
    """
    Endpoint to receive new sensor data in the 'var' parameter (format: key:value,key:value).
    Parses the data, appends it to the data store, and emits to connected WebSocket clients.
    """
    var_value = request.args.get('var')
    if var_value:
        try:
            # Parse incoming data
            data_parts = var_value.split(',')
            data_dict = {}
            for part in data_parts:
                key, value = part.split(':')
                data_dict[key.strip()] = float(value.strip())

            # Add a timestamp
            data_dict["timestamp"] = time.time()

            # Store the data and emit it to WebSocket clients
            data_store.append(data_dict)
            socketio.emit('new_data', data_dict)

            return "Data updated successfully", 200
        except Exception as e:
            return f"Error parsing data: {str(e)}", 400
    else:
        return "No data provided", 400

@app.route('/get_suggestion', methods=['GET'])
def get_suggestion():
    try:
        # Extract sensor data from query parameters
        humidity = request.args.get("humidity")
        temperature = request.args.get("temperature")
        light_value = request.args.get("lightValue")
        soil_moisture = request.args.get("soilMoisture")

        # Check if all parameters are provided
        if not all([humidity, temperature, light_value, soil_moisture]):
            return jsonify({"error": "Missing one or more required parameters."}), 400

        # Create a prompt for OpenAI
        prompt = (
            f"Here are the current plant environment conditions:\n"
            f"- Humidity: {humidity}%\n"
            f"- Temperature: {temperature}Â°C\n"
            f"- Light Value: {light_value} lux\n"
            f"- Soil Moisture: {soil_moisture}%\n\n"
            f"Based on these conditions, provide actionable advice for improving plant health."
        )

        # Use OpenAI ChatCompletion interface
        response = openai.ChatCompletion.create(
            model="gpt-4",
            messages=[
                {"role": "system", "content": "You are a helpful assistant providing plant care advice."},
                {"role": "user", "content": prompt}
            ],
            max_tokens=300
        )

        # Extract the suggestion
        suggestion = response.choices[0].message['content'].strip()
        print(suggestion)
        # Format the suggestion for readability
        #formatted_suggestion = suggestion.replace("\n", "<br>")
        #return render_template("index.html", suggestion=formatted_suggestion)

        # Send the formatted suggestion to the front-end
        return jsonify(suggestion)

    except Exception as e:
        return jsonify({"error": str(e)}), 500
    
@socketio.on('connect')
def handle_connect():
    """
    Handles WebSocket client connections.
    Sends the last N records of sensor data to the client upon connection.
    """
    socketio.emit('initial_data', list(data_store))

if __name__ == '__main__':
    # Run the Flask-SocketIO app
    socketio.run(app, host='0.0.0.0', debug=True)
