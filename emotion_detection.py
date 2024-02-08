import tensorflow as tf
import numpy as np
import cv2
import sys
# Load pre-trained TensorFlow model for emotion detection
model = tf.keras.models.load_model('emotion_detection_model.h5')

# Get the command-line arguments
arguments = sys.argv
image_path = user_arguments = arguments[1]
print(image_path)
# Emotion labels
emotion_labels = ['Angry', 'Disgust', 'Fear', 'Happy', 'Sad', 'Surprise', 'Neutral']

def detect_emotion(image_path):
    # Load image
    image = cv2.imread(image_path)
    # Resize image to match model input shape (48x48 pixels)
    resized_image = cv2.resize(image, (48, 48))
    # Convert image to grayscale
    grayscale_image = cv2.cvtColor(resized_image, cv2.COLOR_BGR2GRAY)
    # Normalize pixel values
    normalized_image = grayscale_image / 255.0
    # Add batch dimension
    input_image = np.expand_dims(normalized_image, axis=0)
    # Perform emotion detection
    predictions = model.predict(input_image)
    # Get predicted emotion label
    predicted_label = emotion_labels[np.argmax(predictions)]
    return predicted_label

if __name__ == "__main__":
    # Example usage
    #image_path = "C:\\Users\\shubh\\OneDrive\\Pictures\\facedetection\\happy3.jpeg"  # Replace with the path to your test image
    emotion = detect_emotion(image_path)
    print("Detected emotion:", emotion)
