#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "structs.h"

// function to get operations array
void get_operations(void ** operations);

// function to read sensor from file
void read_sensor(sensor * sensor, FILE * fin) {
  // read number of operations from file
  fread(&sensor->nr_operations, sizeof(int), 1, fin);
  if (sensor->nr_operations > 0) {
    // allocate memory for operations index array and read from file
    sensor->operations_idxs = (int *)malloc(sizeof(int) * sensor->nr_operations);
    fread(sensor->operations_idxs, sizeof(int), sensor->nr_operations, fin);
  } else {
    sensor->operations_idxs = NULL;
  }
}

// function to allocate and read sensors from file
sensor *allocate_and_read(int nr_sensors, FILE * fin) {
  int i;
  // allocate memory for sensor array
  sensor *s = (sensor *)malloc(sizeof(sensor) * nr_sensors);
  for (i = 0; i < nr_sensors; i++) {
    // read sensor type from file
    fread((int *)&s[i].sensor_type, sizeof(int), 1, fin);
    // allocate memory for sensor data based on sensor type and read from file
    if (s[i].sensor_type == PMU) {
      s[i].sensor_data = (void *)malloc(sizeof(power_management_unit));
      fread((power_management_unit *)s[i].sensor_data, sizeof(power_management_unit), 1, fin);
    } else if (s[i].sensor_type == TIRE) {
      s[i].sensor_data = (void *)malloc(sizeof(tire_sensor));
      fread((tire_sensor *)s[i].sensor_data, sizeof(tire_sensor), 1, fin);
    }
    // read sensor from file
    read_sensor(&s[i], fin);
  }
  return s;
}

// function to print simple sensor information
void simple_print(const char *input) {
  FILE *fin = fopen(input, "rb");
  if (fin == NULL) {
    printf("Error\n");
    return;
  }
  int nr_sensors;
  // read number of sensors from file
  fread(&nr_sensors, sizeof(int), 1, fin);
  // allocate and read sensors from file
  sensor *s = allocate_and_read(nr_sensors, fin);

  char sir[100];
  while (1) {
    // read command from user
    scanf("%s", sir);
    if (strcmp(sir, "exit") == 0) {
      // free memory and exit program
      int k;
      for (k = 0; k < nr_sensors; k++) {
        if (s[k].sensor_data != NULL) {
          free(s[k].sensor_data);
        }
        if (s[k].operations_idxs != NULL) {
          free(s[k].operations_idxs);
        }
      }
      free(s);
      break;
    } else if (strcmp(sir, "print") == 0) {
      // print sensor information based on user input
      int n;
      scanf("%d\n", &n);
      if (n < 0 || n >= nr_sensors) {
        printf("Index not in range!\n");
      } else if (s[n].sensor_type == PMU) {
        power_management_unit *pmu = (power_management_unit *)s[n].sensor_data;
        printf("Power Management Unit\n");
        printf("Voltage: %.2f\n", pmu->voltage);
        printf("Current: %.2f\n", pmu -> current);
        printf("Power Consumption: %.2f\n", pmu -> power_consumption);
        printf("Energy Regen: %d%%\n", pmu -> energy_regen);
        printf("Energy Storage: %d%%\n", pmu -> energy_storage);
      } else if (s[n].sensor_type == TIRE) {
        tire_sensor * tire = (tire_sensor * ) s[n].sensor_data;
        printf("Tire Sensor\n");
        printf("Pressure: %.2f\n", tire -> pressure);
        printf("Temperature: %.2f\n", tire -> temperature);
        printf("Wear Level: %d%%\n", tire -> wear_level);
        if (tire -> performance_score == 0)
          printf("Performance Score: Not Calculated\n");
        else
          printf("Performance Score: %d\n", tire -> performance_score);

      }
    }

  }
  fclose(fin);
}
// used by qsort to compare two sensors
int compare_sensors(const void * a,
  const void * b) {
  const sensor * sensor_a = (sensor * ) a;
  const sensor * sensor_b = (sensor * ) b;

  if (sensor_a -> sensor_type == PMU && sensor_b -> sensor_type == TIRE) {
    return -1;
  } else if (sensor_a -> sensor_type == TIRE && sensor_b -> sensor_type == PMU) {
    return 1;
  } else {
    return 0;
  }
}

// eads sensor data from a binary file 
void complex_print(const char * input) {
  FILE * fin = fopen(input, "rb");
  if (fin == NULL) {
    printf("Error\n");
    return ;
  }

  int nr_sensors;
  fread( & nr_sensors, sizeof(int), 1, fin);

  // allocate memory for the sensors and read in their data
  sensor * s = allocate_and_read(nr_sensors, fin);

  // sort the sensors 
  qsort((void * ) s, nr_sensors, sizeof(sensor), compare_sensors);

  // array of function pointers 
  void( * operations_array[8])(void * );
  get_operations((void ** ) operations_array);

  char sir[100];
  while (1) {
    scanf("%s", sir);
    if (strcmp(sir, "exit") == 0) {
      int k;
      for (k= 0; k < nr_sensors; k++) {
      if (s[k].sensor_data != NULL) {
        free(s[k].sensor_data);
    }
    if (s[k].operations_idxs != NULL) {
        free(s[k].operations_idxs);
    }
}
free(s);

      break;
    } else if (strcmp(sir, "print") == 0) {
      int n;
      scanf("%d\n", & n);
      if (n < 0 || n >= nr_sensors) {
        printf("Index not in range!\n");
      } else if (s[n].sensor_type == PMU) {
        power_management_unit * pmu = (power_management_unit * ) s[n].sensor_data;
        printf("Power Management Unit\n");
        printf("Voltage: %.2f\n", pmu -> voltage);
        printf("Current: %.2f\n", pmu -> current);
        printf("Power Consumption: %.2f\n", pmu -> power_consumption);
        printf("Energy Regen: %d%%\n", pmu -> energy_regen);
        printf("Energy Storage: %d%%\n", pmu -> energy_storage);
      } else if (s[n].sensor_type == TIRE) {
        tire_sensor * tire = (tire_sensor * ) s[n].sensor_data;
        printf("Tire Sensor\n");
        printf("Pressure: %.2f\n", tire -> pressure);
        printf("Temperature: %.2f\n", tire -> temperature);
        printf("Wear Level: %d%%\n", tire -> wear_level);
        if (tire -> performance_score == 0)
          printf("Performance Score: Not Calculated\n");
        else
          printf("Performance Score: %d\n", tire -> performance_score);
      }
    } else if (strcmp(sir, "analyze") == 0) {
  int n, i;
  scanf("%d\n", &n);
  if (n < 0 || n >= nr_sensors) {
    printf("Index not in range!\n");
  } else {
    for (i = 0; i < s[n].nr_operations; i++) {
      // execute the function pointed by the operations_array 
      (*operations_array[s[n].operations_idxs[i]])((void*)s[n].sensor_data);
    }
  }
} 
 else if (strcmp(sir, "clear") == 0) {
      int i, j;
      for (j = 0; j < nr_sensors; j++) {
        int bec = 0;
        if (s[j].sensor_type == PMU) {
          power_management_unit * pmu = (power_management_unit * ) s[j].sensor_data;
          if (pmu -> voltage < 10 || pmu -> voltage > 20 || pmu -> current < -100 || pmu -> current > 100 || pmu -> power_consumption < 0 || pmu -> power_consumption > 1000 || pmu -> energy_regen < 0 || pmu -> energy_regen > 100 || pmu -> energy_storage < 0 || pmu -> energy_storage > 100) {

            free(s[j].operations_idxs);
            free(s[j].sensor_data);
            bec = 1;
          }
        } else if (s[j].sensor_type == TIRE) {
          tire_sensor * tire = (tire_sensor * ) s[j].sensor_data;
          if (tire -> pressure < 19 || tire -> pressure > 28 || tire -> temperature < 0 || tire -> temperature > 120 || tire -> wear_level < 0 || tire -> wear_level > 100) {
            free(s[j].operations_idxs);
            free(s[j].sensor_data);
            bec = 1;
          }
        }
        if (bec) {
          // remove sensor from array 
          for (i = j; i < nr_sensors - 1; i++) {
            s[i] = s[i + 1];
          }
          nr_sensors--;
          j--;
        }
      }
    }
  }
  fclose(fin);
}

int main(int argc, char
  const * argv[]) {

  complex_print(argv[1]);
  return 0;
}