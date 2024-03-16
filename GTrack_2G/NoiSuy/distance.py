import pandas as pd
import numpy as np

def haversine(lat1, lon1, lat2, lon2):
    """
    Calculate the great circle distance between two points
    on the earth specified in decimal degrees.
    """
    # Convert decimal degrees to radians
    lat1, lon1, lat2, lon2 = map(np.radians, [lat1, lon1, lat2, lon2])

    # Haversine formula
    dlon = lon2 - lon1
    dlat = lat2 - lat1
    a = np.sin(dlat / 2) ** 2 + np.cos(lat1) * np.cos(lat2) * np.sin(dlon / 2) ** 2
    c = 2 * np.arcsin(np.sqrt(a))
    
    # Radius of earth in kilometers
    r = 6371
    distance = r * c
    return distance

# Read data from CSV files
df1 = pd.read_csv('interpolated_lat_long.csv')
df2 = pd.read_csv('output.csv')

# Calculate distances and store in a new DataFrame
distances = []
for idx1, row1 in df1.iterrows():
    lat1, lon1 = row1['Lat'], row1['Lon']
    for idx2, row2 in df2.iterrows():
        lat2, lon2 = row2['Lat'], row2['Lon']
        distance = haversine(lat1, lon1, lat2, lon2)
        distances.append(distance)

# Create a DataFrame with distances
df_distances = pd.DataFrame({'Distance': distances})

# Save the DataFrame to a CSV file
df_distances.to_csv('distances.csv', index=False)
