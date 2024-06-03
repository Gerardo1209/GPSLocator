const express = require('express');
const app = express();
const cors = require('cors');
const fs = require('fs').promises;
const mysql = require('mysql2/promise');
const { error } = require('console');

//Puerto
const PORT = process.env.PORT || 3000;

//Base de datos
const config = {
    host: 'localhost',
    user: 'gerardo',
    database: 'locaciones_electronica',
    password: 'FDelgadoG1209M#',
    port: '3306'
}

async function conexionBD(){
    try{
        const connection = await mysql.createConnection(config);
        console.log("Se establecio la conexión con la base de datos");
        return connection;
    }catch(err){
        console.log("Error al conectar con la base de datos: ", err);
    }
}


//Configuración
app.use(express.json());
app.use(express.urlencoded({extended: true}));
app.use(cors());

/*async function checkFileExist(file){
    try{
        await fs.readFile(file);
        return true;
    }catch(err){
        if(err.code == "ENOENT"){
            await fs.writeFile(file, JSON.stringify([]));
            return true;
        }else{
            return false;
        }
    }
}*/

app.get('/location/:locationString', async (req, res) => {
    //console.log(req.params.locationString);
    var data = req.params.locationString.split(',');
    var lat = data[2];
    var posLat = data[3];
    var lon = data[4];
    var posLon = data[5];
    var resultLat;
    var resultLon;
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
    var ruta = __dirname + "/../data/gpsData.json";
    //Almacenamiento en archivos
    /*if(await checkFileExist(ruta)){
        var locations = await fs.readFile(ruta);
        var posiciones = JSON.parse(locations);
        posiciones.push({
            fecha: new Date().toISOString(),
            latitud: resultLat,
            longitud: resultLon
        });
        await fs.writeFile(ruta, JSON.stringify(posiciones));
    }else{
        res.send({success: false, data: "Error al abrir el archivo contenedor del GPS"});
    }*/

    //Almacenamiento en base de datos
    if(resultLat && resultLon){
        try{
            await conexionBD().then(async conexion =>{
                const result = await conexion.query(
                    'INSERT INTO Ubicacion (Fecha, Altitud, Longitud) VALUES (?,?,?);',
                    [new Date(), resultLat, resultLon]
                );
                console.log(new Date().toISOString() + ": Se escribieron datos correctamente");
                res.send({success: true, data: "Datos recibidos: "+req.params.locationString});
            }).catch(error => {
                res.send({success: false, data: error.message});
            })
            
        }catch(err){
            res.send({success: false, data: err.message});
        }
    }else{
        res.send({success:false, data: "No se enviaron datos vacios"});
    }
});

app.get('/consult', async (req,res) => {
    try{
        await conexionBD().then(async conexion =>{
            const result = await conexion.query(
                'SELECT * FROM Ubicacion;'
            );
            res.send({success: true, data: result[0]});
        }).catch(error => {
            res.send({success: false, data: error.message});
        })
        
    }catch(err){
        res.send({success: false, data: err.message});
    }
});

app.listen(PORT, ()=>{
    console.log("Servidor escuchando en: " + PORT);
})
