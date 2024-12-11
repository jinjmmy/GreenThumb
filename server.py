from flask import Flask, request
import openai
from dotenv import load_dotenv
import os

load_dotenv()
openai.api_key = os.getenv("OPENAI_API_KEY")

app = Flask(__name__)

@app.route("/", methods=["GET"])
def hello():
    humidity = request.args.get("humidity")
    temperature = request.args.get("temperature")
    light_value = request.args.get("lightValue")
    soil_moisture = request.args.get("soilMoisture")
    
    if not (humidity and temperature and light_value and soil_moisture):
        return "Error: Missing sensor data in request", 400

    prompt = f"""
    I have the following sensor readings:
    - Humidity: {humidity}% (Relative Humidity)
    - Temperature: {temperature}°C
    - Soil Moisture: {soil_moisture}% (Soil Moisture)
    - Light Intensity: {light_value} (Raw sensor value, can be calibrated to lux)

    What should I do to maintain the best conditions for my plant based on these values?
    """

    try:
        response = openai.ChatCompletion.create(
            model="gpt-3.5-turbo", 
            messages=[
                {"role": "system", "content": "You are a helpful assistant."},
                {"role": "user", "content": prompt}
            ]
        )

        return response['choices'][0]['message']['content']

    except openai.error.OpenAIError as e:
        return f"Error occurred: {e}", 500

if __name__ == "__main__":
    app.run(debug=True)
