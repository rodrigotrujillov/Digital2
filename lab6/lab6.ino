
#include <SPI.h>
#include <SD.h>

File myFile;
File root;

char opcion = '0';

void setup()
{
  delay(250);
  SPI.setModule(0);
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ;
  }
  Serial.println("Initializing SD card...");
  pinMode(A3, OUTPUT);

  if (!SD.begin(A3)) {
    Serial.println("initialization failed!");
    return;
  }

  Serial.println("Escoge una opcion");
  Serial.println("1. Leer un archivo existente");
  Serial.println("2. Crear y guardar un archivo nuevo");
}

void loop()
{
  if (Serial.available()) {
    char input = Serial.read();
    if (input >= '1' && input <= '2') {
      opcion = input; // Colocamos la opcion deseada
      if (opcion == '1') {
        Serial.println('\n');
        root = SD.open("/");
        printDirectory(root, 0);
        Serial.println("Escribe el nombre del archivo que deseas visualizar");
      } else if (opcion == '2') {
        Serial.println("Escribe el nombre del archivo que deseas crear (ej. nuevo.txt)");
      }
    } else if (opcion != '0') { // Ya se selecciono una opcion
      if (opcion == '1') {
        // Que se lea un archivo ya existente
        String archivo = input + Serial.readStringUntil('\n');
        readAndPrintFile(archivo.c_str());
        opcion = '0'; // Reseteamos opcion
      } else if (opcion == '2') {
        // Que se cree un archivo nuevo y guardarlo
        String nuevoArchivo = input + Serial.readStringUntil('\n');
        writeAndSaveFile(nuevoArchivo.c_str());
        opcion = '0'; // Reseteamos opcion
      } else {
        Serial.println("Invalido");
      }
    } else {
      Serial.println("Invalido");
    }
  }
}


/********************************************/
void readAndPrintFile(const char* filename)
{
  myFile = SD.open(filename);
  if (myFile) {
    Serial.print(filename); 
    Serial.println(":");
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    myFile.close();
  } else {
    Serial.print("Error al iniciar ");
    Serial.println(filename);
  }
}

void writeAndSaveFile(const char* filename)
{
  String data;
  Serial.println("Ingresa el texto del ASCII, asegura que sea de 80 caracteres de largo para que funcione correctamente");
  while (!Serial.available()) {
    //Esperamos input del usuario
  }
  data = Serial.readStringUntil('\n');
  
  myFile = SD.open(filename, FILE_WRITE);
  if(myFile) {
    Serial.print("Escribiendo archivo:");
    Serial.println(filename);

    int charContador = 0;
    for (int i = 0; i < data.length(); i++)
    {
      myFile.print(data[i]);
      charContador++;
      if (charContador == 81)
      {
        myFile.println();
        charContador = 0;
      }
    }
    myFile.close();
    Serial.println("Hecho.");
  }
  else {
    Serial.print("Error creando archivo: ");
    Serial.println(filename);
  }
}

void printDirectory(File dir, int numTabs) {
   while(true) {
     
     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}
