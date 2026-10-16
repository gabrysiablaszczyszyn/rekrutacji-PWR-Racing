import pandas as pd
import matplotlib.pyplot as plt

dane1 = pd.read_csv('sensor1.csv')
dane2 = pd.read_csv('sensor2.csv')

plt.figure(figsize=(10, 8))
plt.scatter(dane1['x'][0], dane1['y'][0])
plt.plot(dane1['x'], dane1['y'], label='Sensor 1', color='blue', marker='.', linestyle='-', markersize=3)
plt.plot(dane2['x'], dane2['y'], label='Sensor 2', color='red', marker='.', linestyle='-', markersize=3, alpha=0.7)

plt.title('Porównanie odczytów trasy bolidu')
plt.xlabel('Oś X - Przemieszczenie w przód')
plt.ylabel('Oś Y - Przemieszczenie w lewo[')
plt.legend()
plt.grid(True)

plt.axis('equal') 
plt.show()