import pandas as pd
import matplotlib.pyplot as plt
from flask import Flask, render_template, send_file, request, jsonify
import os
import json
# import joblib
import subprocess

plt.switch_backend('Agg')
app = Flask(__name__)

# Load the trained model
# model = joblib.load('crop_prediction_model.pkl')

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

# @app.route('/')
# def index():
#     return render_template('crop_prediction.html')

@app.route('/predict', methods=['GET', 'POST'])
def predict():
    if request.method == 'POST':
        # Get the data from the form
        N = request.form['N']
        P = request.form['P']
        K = request.form['K']
        temperature = request.form['temperature']
        humidity = request.form['humidity']
        ph_level = request.form['ph_level']
        rainfall = request.form['rainfall']

        # Create a dictionary to store the data
        instance_data = {
            'N': N,
            'P': P,
            'K': K,
            'temperature': temperature,
            'humidity': humidity,
            'ph_level': ph_level,
            'rainfall': rainfall
        }

        # Write the data to a JSON file
        with open('instance.json', 'w') as f:
            json.dump(instance_data, f)

        # Compile and execute the C++ program
        cpp_file = 'predict.cpp'
        executable_file = cpp_file[:-4]  # Remove .cpp extension
        compile_command = f"g++ {cpp_file} -o {executable_file}"
        subprocess.run(compile_command, shell=True)

        # Run the compiled C++ program in a subprocess
        terminal_command = f"./{executable_file}"
        result = subprocess.run(terminal_command, capture_output=True, text=True)

        # Parse the result from C++ program output
        predicted_crop = result.stdout.strip()  # Assuming C++ program outputs the predicted crop

        # Return the predicted crop as JSON response
        # return jsonify({'predicted_crop': predicted_crop})
        return render_template('crop_prediction.html', predicted_crop=predicted_crop)
    return render_template('crop_prediction.html')

    # return render_template('crop_prediction.html')

# Load unique crop names from price_avg.csv
crop_names = set()
with open('price_avg.csv', 'r') as f:
    next(f)  # Skip header
    for line in f:
        crop_name, _ = line.strip().split(',')
        crop_names.add(crop_name)

@app.route('/profit', methods=['GET', 'POST'])
def profit():
    if request.method == 'POST':
        crop_name = request.form['crop_name']
        # Run the C++ program with the selected crop name
        output = subprocess.check_output(['./profit_predict', crop_name])
        return render_template('crop_profit.html', output=output.decode('utf-8'))
    return render_template('crop_profit.html', crop_names=crop_names)

if __name__ == '__main__':
    if not os.path.exists('static'):
        os.makedirs('static')
    app.run(debug=True)
