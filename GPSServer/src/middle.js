import fetch from 'node-fetch';
import mysql from 'mysql2/promise';
import config from './bd.js';

async function conexionBD() {
  try {
    const connection = await mysql.createConnection(config);
    console.log("Se estableció la conexión con la base de datos");
    return connection;
  } catch (err) {
    console.log("Error al conectar con la base de datos: ", err);
  }
}

async function fetchData() {
  try {
    const response = await fetch('https://electronica.femat.dev/consult');
    const data = await response.json();
    return data;
  } catch (error) {
    console.error(error);
  }
}

export { conexionBD, fetchData };