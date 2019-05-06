# DomoticaPrototype

Prototipo de la alternativa de proyecto N°1 del Grupo 1 de la materia Ingeniería de Software 2019, UTN Facultad Regional Delta.

## El Proyecto

El proyecto consiste de un sistema de domótica que permite el control y la automatización de los aparatos eléctricos de un lugar. En nuestro caso, pensamos en su aplicación práctica dentro de la facultad.

## Arquitectura del prototipo

<p align="center"> <img src="https://github.com/Morlax97/DomoticaPrototype/blob/master/Prototype.png" alt="Prototype Architecture"> </p>

En su versión actual el prototipo está compuesto por un servidor, una Raspberry Pi 2 corriendo Raspbian Stretch Lite con los siguientes servicios:

  * Nginx Web Server - Usado como hosting de la GUI
  * NodeJS - Entorno sobre el cual está hecho el backend.
  * MariaDB - Base de datos accedida por el backend para guardar información sobre los dispositivos.
  * Mosquitto - Servidor MQTT que permite la comunicación entre el backend y los dispositivos.

## Funcionamiento del prototipo

Al iniciar el servidor, se suscribe al canal de MQTT por el cual va a recibir los mensajes del dispositivo. También empieza a escuchar en el puerto 3000 tanto en UDP como TCP.

Al encender el microcontrolador, este se conecta a la red inalámbrica que tenga configurada, y luego hace un broadcast al puerto 3000 UDP informando su ID y pidiendo configuración. El servidor recibe este broadcast, busca en la base de datos la configuración del microcontrolador según su ID y le devuelve un JSON con la configuración de sus pines. El microcontrolador recibe el JSON, lo parsea, y lo configura sus pines. Por último, se suscribe al canal por el cual el servidor le comandará encenderse o apagarse.
Al llamar al método POST /toggledevice con el ID del dispositivo, el servidor obtiene el estado actual del microcontrolador y envía un mensaje MQTT al mismo para cambiar su estado.

Al presionar el botón conectado a un pin del microcontrolador, este entra en modo "override", encendiendo el led e impidiendo que sea apagado por el servidor. Al presionar nuevamente el botón, se sale del modo "override" y se apaga manualmente el led, recibiendo comandos nuevamente.

## Configuración

Pasos a seguir para configurar el servidor.

### Instalar dependencias

```bash
sudo apt update
sudo apt upgrade -y
sudo apt install nodejs npm mysql-server nginx git mosquitto mosquitto-clients -y
```

### Configurar base de datos

```bash
sudo mysql
	CREATE DATABASE restapi;
	GRANT ALL PRIVILEGES ON restapi.* TO apiuser@localhost IDENTIFIED BY 'apipwd';
	FLUSH PRIVILEGES;
	quit;

mysql -u apiuser --password=apipwd
  USE restapi;
  CREATE TABLE Devices (
  DevicesID INT AUTO_INCREMENT PRIMARY KEY,
	nombre VARCHAR(30) NOT NULL,
	status BOOLEAN,
  setup VARCHAR(200) NOT NULL,
	);
  INSERT INTO Sensores(nombre,status,setup)
  VALUES('led',false,'[{"pin":13,"mode":"OUTPUT"},{"pin":14,"mode":"INPUT_PULLUP"}]')
	quit;
```
### Configurar git y descargar proyecto

```bash
mkdir prototype
cd prototype
git init
git config --global user.name USERNAME
git config --global user.email EMAIL
git remote add origin https://github.com/Morlax97/DomoticaPrototype.git
git pull origin master
```
Iniciar el servidor mediante ``` node /home/pi/prototype/nodejs/restapi/index.js ```

### Configurar Web GUI

Pendiente.
