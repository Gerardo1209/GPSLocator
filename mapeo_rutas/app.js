import express from 'express';
import fetch from 'node-fetch';
import path from 'path';

// Rest of your code remains the same
const app = express();
const port = 3000;

// Configurar EJS como motor de vistas
app.set('view engine', 'ejs');
app.set('views', path.join(process.cwd(), 'views'));

// Servir archivos estáticos (por ejemplo, Leaflet CSS y JS)
app.use(express.static('public'));

// Ruta principal
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

app.listen(port, () => {
  console.log(`Servidor escuchando en http://localhost:${port}`);
});
