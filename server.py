import openai
from dotenv import load_dotenv
import os


load_dotenv()
openai.api_key = os.getenv("OPENAI_API_KEY")

# Sensor data and units
humidity = "45.6"  
temperature = "22.3"  
soil_moisture = "60"  
light_value = "550"  

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
        model="gpt-3.5-turbo",  # Use the newer model
        messages=[
            {"role": "system", "content": "You are a helpful assistant."},
            {"role": "user", "content": prompt}
        ]
    )

    print(response['choices'][0]['message']['content'])
except openai.error.OpenAIError as e:
    print(f"Error occurred: {e}")
