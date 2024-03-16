import pandas as pd
from geopy.distance import geodesic
import numpy as np


# Đọc dữ liệu từ hai tập CSV
df_standard = pd.read_csv('interpolated_spline.csv')  # Dữ liệu lat long chuẩn
# df_standard = pd.read_csv('interpolated_spline.csv')  # Dữ liệu lat long chuẩn
df_sensor = pd.read_csv('output.csv')      # Dữ liệu lat long đo được

# Loại bỏ các hàng chứa giá trị NaN
df_standard = df_standard.dropna()
df_sensor = df_sensor.dropna()

# Tính toán sai số giữa các cặp điểm
errors = []
for index, row in df_standard.iterrows():
    lat_standard, lon_standard = row['Lat'], row['Lon']
    # Kiểm tra xem chỉ số index có trong index của df_sensor hay không
    if index in df_sensor.index:
        lat_sensor, lon_sensor = df_sensor.loc[index, 'Lat'], df_sensor.loc[index, 'Lon']
        distance = geodesic((lat_standard, lon_standard), (lat_sensor, lon_sensor)).kilometers
        errors.append(distance)
    else:
        print(f"Warning: Index {index} is not in the index of df_sensor.")

# Lưu sai số vào DataFrame
df_errors = pd.DataFrame({'Error (km)': errors})

# Ghi DataFrame vào file CSV
df_errors.to_csv('errors.csv', index=False)

# Đọc dữ liệu từ tập tin errors.csv (nếu chưa được đọc)
df_errors = pd.read_csv('errors.csv')

# Tính trung bình bình phương sai số
mse = np.mean(df_errors['Error (km)'] ** 2)
print("Mean Squared Error (MSE):", mse)


