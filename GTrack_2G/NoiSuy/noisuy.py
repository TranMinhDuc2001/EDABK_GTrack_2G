import pandas as pd
import numpy as np

# Đọc dữ liệu từ file CSV
df_data1 = pd.read_csv('data_3s.csv')  # Dữ liệu được lấy mẫu mỗi 3 giây (1 mẫu)
df_data2 = pd.read_csv('output.csv')  # Dữ liệu được lấy mẫu mỗi 1 giây (19 mẫu)

# Tạo dữ liệu chỉ số thời gian cho dữ liệu mỗi mili giây
num_rows_data1 = len(df_data1) * 88  # Số lượng dữ liệu cần tăng lên để bằng số lượng trong output.csv
idx_data1 = np.linspace(0, len(df_data1) - 1, num_rows_data1)

# Nội suy dữ liệu
df_interp_data1 = pd.DataFrame()
df_interp_data1['Lat'] = np.interp(idx_data1, np.arange(len(df_data1)), df_data1['Lat'])
df_interp_data1['Lon'] = np.interp(idx_data1, np.arange(len(df_data1)), df_data1['Lon'])

# Giảm số lượng dữ liệu xuống để bằng với số lượng trong output.csv
df_interp_data1 = df_interp_data1.iloc[:90153]

# Lưu dữ liệu vào file CSV mới
df_interp_data1.to_csv('interpolated_liner.csv', index=False)



# import pandas as pd
# import numpy as np
# from scipy.interpolate import interp1d

# # Đọc dữ liệu từ file mẫu và file chuẩn
# df_sample = pd.read_csv('data_3s.csv')
# df_standard = pd.read_csv('output.csv')

# # Xác định số lượng điểm dữ liệu trong mỗi tập tin
# num_points_sample = len(df_sample)
# num_points_standard = len(df_standard)

# # Tạo hàm nội suy spline cho vĩ độ và kinh độ
# spline_interp_lat = interp1d(range(num_points_sample), df_sample['Lat'], kind='cubic')
# spline_interp_lon = interp1d(range(num_points_sample), df_sample['Lon'], kind='cubic')

# # Tạo dữ liệu mới để nội suy
# new_indices = np.linspace(0, num_points_sample - 1, num_points_standard)

# # Nội suy dữ liệu
# new_latitudes = spline_interp_lat(new_indices)
# new_longitudes = spline_interp_lon(new_indices)

# # Tạo DataFrame cho dữ liệu nội suy
# df_interpolated = pd.DataFrame({'Lat': new_latitudes, 'Lon': new_longitudes})

# # Lưu dữ liệu nội suy vào file CSV
# df_interpolated.to_csv('interpolated_spline.csv', index=False)

