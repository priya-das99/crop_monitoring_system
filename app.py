import pandas as pd
import matplotlib.pyplot as plt
from flask import Flask, render_template, send_file, request
import os
import json
import joblib

plt.switch_backend('Agg')
app = Flask(__name__)

# Load the trained model
model = joblib.load('crop_prediction_model.pkl')

def generate_plot():
    df = pd.read_csv("sensor_data.csv")

    plt.figure(figsize=(6, 4))
    plt.plot(df.index, df['soil_moisture_level'], label = 'Soil Moisture Level')
    plt.plot(df.index, df['ph_level'], label = 'PH Level')
    plt.xlabel('Time')
    plt.ylabel('Value')
    plt.legend()


    plot_filename = os.path.join('static', 'plot.png')
    plt.savefig(plot_filename)
    plt.close()
    return plot_filename

# @app.route('/')
# def index():
#     generate_plot()
#     return render_template("index.html")

@app.route('/')
def index():
    with open('data.json') as f:
        data = json.load(f)
    generate_plot()
    return render_template('index.html', data=data)

@app.route('/plot.png')
def plot_png():
    plot_filename = generate_plot()
    return send_file(plot_filename, mimetype='image/png')

@app.route('/predict', methods=['GET', 'POST'])
def predict():
    if request.method == 'POST':
        # Get user inputs from the form
        N = float(request.form['N'])
        P = float(request.form['P'])
        K = float(request.form['K'])
        temperature = float(request.form['temperature'])
        humidity = float(request.form['humidity'])
        ph_level = float(request.form['ph_level'])
        rainfall = float(request.form['rainfall'])

        # Prepare input data as a DataFrame
        input_data = pd.DataFrame([[N, P, K, temperature, humidity, ph_level, rainfall]],
                                  columns=['N', 'P', 'K', 'temperature', 'humidity', 'ph_level', 'rainfall'])

        # Use the model to make a prediction
        prediction = model.predict(input_data)
        predicted_crop = prediction[0]

        return render_template('crop_prediction.html', predicted_crop=predicted_crop)

    return render_template('crop_prediction.html')

if __name__ == '__main__':
    if not os.path.exists('static'):
        os.makedirs('static')
    app.run(debug=True)
