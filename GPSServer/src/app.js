import express from 'express';
import fetch from 'node-fetch';
import path from 'path';
import cors from 'cors';
import mysql from 'mysql2/promise';
import dotenv from 'dotenv';

dotenv.config();

const app = express();
const port = process.env.PORT || 3000;

// Configuración de la base de datos
const config = {
    host: process.env.DB_HOST || 'uaaantojos.femat.dev',
    user: process.env.DB_USER || 'gerardo',
    database: process.env.DB_NAME || 'locaciones_electronica',
    password: process.env.DB_PASSWORD || 'FDelgadoG1209M#',
    port: process.env.DB_PORT || '3306'
};

async function conexionBD() {
    try {
        const connection = await mysql.createConnection(config);
        console.log("Se estableció la conexión con la base de datos");
        return connection;
    } catch (err) {
        console.log("Error al conectar con la base de datos: ", err);
    }
}

// Configuración
app.use(express.json());
app.use(express.urlencoded({ extended: true }));
app.use(cors());

// Configurar EJS como motor de vistas
app.set('view engine', 'ejs');
app.set('views', path.resolve('src', 'views')); // Ruta actualizada para el directorio de vistas

// Servir archivos estáticos (por ejemplo, Leaflet CSS y JS)
app.use(express.static(path.join(process.cwd(), 'public')));

// Ruta principal para la vista
app.get('/', async (req, res) => {
    try {
        const response = await fetch('https://electronica.femat.dev/consult');
        const data = await response.json();
        if (data.success) {
            res.render('index', { locations: data.data });
        } else {
            res.render('index', { locations: [] });
        }
    } catch (error) {
        console.error(error);
        res.render('index', { locations: [] });
    }
});

// Ruta API para recibir ubicaciones
app.get('/location/:locationString', async (req, res) => {
    const data = req.params.locationString.split(',');
    const lat = parseFloat(data[0]); // Latitud
    const posLat = data[1];
    const lon = parseFloat(data[2]); // Longitud
    const posLon = data[3];
    const date = data[4]; // Fecha
    const time = data[5]; // Hora

    let resultLat;
    let resultLon;
    let timestamp;

    // Convertir la latitud y la longitud a grados decimales
    if (isNaN(lat) || isNaN(lon)) {
        res.status(400).send({ success: false, data: "Coordenadas inválidas" });
        return;
    }

    if(lat){
        var dda = lat / 100;
        dda = parseInt(dda.toString());
        var ssa = lat - dda * 100;
        var decLat = dda + ssa / 60;
        if(posLat == "S"){
            decLat = decLat * -1;
        }
        resultLat = decLat.toFixed(8);
        //var ss = c
    }
    if(lon){
        var ddo = lon / 100;
        ddo = parseInt(ddo.toString());
        var sso = lon - ddo * 100;
        var decLon = ddo + sso / 60;
        if(posLon == "W"){
            decLon = decLon * -1;
        }
        resultLon = decLon.toFixed(8);
    }

    // Construir el timestamp
    if (date && time) {
        const [day, month, year] = date.match(/\d{2}/g);
        const [hour, minute, second] = time.match(/\d{2}/g);
        timestamp = new Date(`20${year}-${month}-${day}T${hour}:${minute}:${second}`);
    } else {
        res.status(400).send({ success: false, data: "Fecha o hora inválidas" });
        return;
    }

    try {
        // Insertar los datos en la base de datos
        const connection = await conexionBD();
        await connection.query(
            'INSERT INTO Ubicacion (Fecha, Altitud, Longitud) VALUES (?,?,?);',
            [timestamp, resultLat, resultLon]
        );
        console.log(new Date().toISOString() + ": Se escribieron datos correctamente");
        res.send({ success: true, data: "Datos recibidos: " + req.params.locationString });
    } catch (err) {
        res.status(500).send({ success: false, data: err.message });
    }
});

// Ruta API para consultar ubicaciones
app.get('/consult', async (req, res) => {
    try {
        const connection = await conexionBD();
        const [rows] = await connection.query('SELECT * FROM Ubicacion;');
        res.send({ success: true, data: rows });
    } catch (err) {
        res.send({ success: false, data: err.message });
    }
});

app.listen(port, () => {
    console.log('Servidor escuchando en http://localhost:' + port);
});
