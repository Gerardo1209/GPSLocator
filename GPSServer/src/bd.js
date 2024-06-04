import dotenv from 'dotenv';
import mysql from 'mysql2/promise';

dotenv.config();

const config = {
  host: process.env.DB_HOST || 'uaaantojos.femat.dev',
  user: process.env.DB_USER || 'gerardo',
  database: process.env.DB_NAME || 'locaciones_electronica',
  password: process.env.DB_PASSWORD || 'FDelgadoG1209M#',
  port: process.env.DB_PORT || '3306'
};

export default config;