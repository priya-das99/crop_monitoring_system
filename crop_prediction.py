import pandas as pd
from sklearn.tree import DecisionTreeClassifier
# from sklearn.model_selection import train_test_split
# from sklearn.metrics import accuracy_score
import joblib
# from sklearn.preprocessing import LabelEncoder

# Load dataset
df = pd.read_csv('Crop_recommendation.csv')
X = df.drop(columns=['label'])
y = df['label']

# label_encoder = LabelEncoder()
# target_encoded = label_encoder.fit_transform(y)
# Split data into training and testing sets
# X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

# Train the decision tree model
model = DecisionTreeClassifier(criterion="entropy",random_state=2,max_depth=5)
model.fit(X, y)

# Save the model for later use
joblib.dump(model, 'crop_prediction_model.pkl')
