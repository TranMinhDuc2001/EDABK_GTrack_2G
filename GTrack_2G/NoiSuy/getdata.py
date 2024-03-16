import pandas as pd

# Đọc dữ liệu từ file CSV
df = pd.read_csv('tableview_240316_015459.csv')

# Trích xuất cột 'Lat' và 'Lon'
latitudes = df['Lat']
longitudes = df['Lon']

# Lưu vào file CSV mới
output_df = pd.DataFrame({'Lat': latitudes, 'Lon': longitudes})
output_df.to_csv('output.csv', index=False)  # Đặt index=False để không ghi cột index vào tệp CSV
