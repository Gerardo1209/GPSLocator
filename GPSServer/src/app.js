import express from 'express';
import fetchRoutes from './routes.js';

const app = express();
app.use(express.json());

// Rutas
fetchRoutes(app);

const port = process.env.PORT || 3000;
app.listen(port, () => {
  console.log(`Servidor escuchando en http://localhost:${port}`);
});