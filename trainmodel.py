import pandas as pd
import numpy as np
from sklearn.linear_model import LinearRegression
import json

# Загружаем данные
df = pd.read_csv('fishing_data.csv')

# Признаки (9 штук)
features = [
    'air_temperature', 'pressure', 'water_temperature',
    'wind_speed', 'wind_direction', 'time_of_day',
    'season', 'moon_phase', 'recent_activity'
]

X = df[features]
y = df['catch_weight'] / df['catch_weight'].max()  # нормализация

# Обучаем модель
model = LinearRegression()
model.fit(X, y)

# Сохраняем коэффициенты
coef_list = model.coef_.tolist()
intercept = model.intercept_
max_weight = float(df['catch_weight'].max())

# Записываем в JSON
with open('model_coefficients.json', 'w') as f:
    json.dump({
        'coefficients': coef_list,
        'intercept': intercept,
        'max_weight': max_weight,
        'features': features
    }, f, indent=2)

print("Модель обучена и сохранена!")
print(f"R² = {model.score(X, y):.3f}")
