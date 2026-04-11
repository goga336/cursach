# import pandas as pd
# import numpy as np
# from sklearn.linear_model import LinearRegression
# import json

# # Загружаем данные
# df = pd.read_csv('fishing_data.csv')

# # Признаки (9 штук)
# features = [
#     'air_temperature', 'pressure', 'water_temperature',
#     'wind_speed', 'wind_direction', 'time_of_day',
#     'season', 'moon_phase', 'recent_activity'
# ]

# X = df[features]
# y = df['catch_weight'] / df['catch_weight'].max()  # нормализация

# # Обучаем модель
# model = LinearRegression()
# model.fit(X, y)

# # Сохраняем коэффициенты
# coef_list = model.coef_.tolist()
# intercept = model.intercept_
# max_weight = float(df['catch_weight'].max())

# # Записываем в JSON
# with open('model_coefficients.json', 'w') as f:
#     json.dump({
#         'coefficients': coef_list,
#         'intercept': intercept,
#         'max_weight': max_weight,
#         'features': features
#     }, f, indent=2)

# print("Модель обучена и сохранена!")
# print(f"R² = {model.score(X, y):.3f}")


# import pandas as pd
# import numpy as np
# from sklearn.linear_model import LinearRegression
# from sklearn.model_selection import train_test_split, cross_val_score
# from sklearn.metrics import r2_score
# import json

# # Загружаем данные
# #df = pd.read_csv('fishing_data_augmented_1000.csv')
# df = pd.read_csv('fishing_data_augmented.csv')

# #df = pd.read_csv('fishing_data_augmented_50.csv')
# #df = pd.read_csv('fishing_data.csv')

# #df = pd.read_csv('fishing_data_augmented_50_300.csv')

# # Признаки
# features = [
#     'air_temperature', 'pressure', 'water_temperature',
#     'wind_speed', 'wind_direction', 'time_of_day',
#     'season', 'moon_phase', 'recent_activity'
# ]

# X = df[features]
# y = df['catch_weight'] / df['catch_weight'].max()

# # Обучаем модель
# model = LinearRegression()
# model.fit(X, y)

# # Метрики
# y_pred = model.predict(X)
# r2 = r2_score(y, y_pred)

# # Кросс-валидация
# cv_scores = cross_val_score(model, X, y, cv=5)

# # Разделение для проверки переобучения
# X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)
# model.fit(X_train, y_train)
# train_r2 = model.score(X_train, y_train)
# test_r2 = model.score(X_test, y_test)

# # Сохраняем
# with open('model_coefficients.json', 'w') as f:
#     json.dump({
#         'coefficients': model.coef_.tolist(),
#         'intercept': model.intercept_,
#         'max_weight': float(df['catch_weight'].max()),
#         'features': features,
#         'metrics': {
#             'r2': float(r2),
#             'train_r2': float(train_r2),
#             'test_r2': float(test_r2),
#             'gap': float(train_r2 - test_r2),
#             'cv_mean': float(cv_scores.mean()),
#             'cv_std': float(cv_scores.std())
#         }
#     }, f, indent=2)

# print(f"R² = {r2:.3f}")
# print(f"Train R² = {train_r2:.3f}")
# print(f"Test R² = {test_r2:.3f}")
# print(f"Gap = {train_r2 - test_r2:.3f}")
# print(f"CV = {cv_scores.mean():.3f} ± {cv_scores.std():.3f}")

import pandas as pd
import numpy as np
from sklearn.linear_model import LinearRegression
from sklearn.model_selection import train_test_split, cross_val_score
from sklearn.metrics import r2_score
import json

# Загружаем данные
#df = pd.read_csv('fishing_data.csv')
df = pd.read_csv('fishing_data_augmented.csv')

# Признаки
features = [
    'air_temperature', 'pressure', 'water_temperature',
    'wind_speed', 'wind_direction', 'time_of_day',
    'season', 'moon_phase', 'recent_activity'
]

# === НОРМАЛИЗАЦИЯ ПРИЗНАКОВ (как в C++) ===
def normalize_column(df, col, min_val, max_val):
    df[col] = (df[col] - min_val) / (max_val - min_val)

# Нормализуем каждый признак
normalize_column(df, 'air_temperature', -50, 50)
normalize_column(df, 'pressure', 700, 800)
normalize_column(df, 'water_temperature', 0, 30)
normalize_column(df, 'wind_speed', 0, 50)
# Остальные признаки уже нормализованы в C++ (0-1)

X = df[features]
y = df['catch_weight'] / df['catch_weight'].max()

# Обучаем модель
model = LinearRegression()
model.fit(X, y)

# Метрики
y_pred = model.predict(X)
r2 = r2_score(y, y_pred)

# Кросс-валидация
cv_scores = cross_val_score(model, X, y, cv=5)

# Разделение для проверки переобучения
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)
model.fit(X_train, y_train)
train_r2 = model.score(X_train, y_train)
test_r2 = model.score(X_test, y_test)

# Сохраняем
with open('model_coefficients.json', 'w') as f:
    json.dump({
        'coefficients': model.coef_.tolist(),
        'intercept': model.intercept_,
        'max_weight': float(df['catch_weight'].max()),
        'features': features,
        'metrics': {
            'r2': float(r2),
            'train_r2': float(train_r2),
            'test_r2': float(test_r2),
            'gap': float(train_r2 - test_r2),
            'cv_mean': float(cv_scores.mean()),
            'cv_std': float(cv_scores.std())
        }
    }, f, indent=2)

print(f"Модель сохранена! Max weight: {df['catch_weight'].max()}")
print(f"R² = {r2:.3f}")