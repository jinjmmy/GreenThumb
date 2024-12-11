from dotenv import load_dotenv
import os
import openai
# Load the .env file
load_dotenv()

# Access the environment variable
openai.api_key = os.getenv("OPENAI_API_KEY")


try:
    response = openai.ChatCompletion.create(
        model="gpt-3.5-turbo",  # Use the newer model
        messages=[
            {"role": "system", "content": "You are a helpful assistant."},
            {"role": "user", "content": "Hello, world!"}
        ]
    )
    print(response['choices'][0]['message']['content'])
except openai.error.OpenAIError as e:
    print(f"Error occurred: {e}")
