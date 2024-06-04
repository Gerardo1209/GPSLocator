<!DOCTYPE html>
<html>
<head>
  <title>Mapa de Rutas</title>
  <link rel="stylesheet" href="https://unpkg.com/leaflet/dist/leaflet.css" />
  <style>
    #map {
      height: 600px;
      width: 100%;
    }
  </style>
</head>
<body>
  <h1>Mapa de Rutas</h1>
  <div id="map"></div>
  <script src="https://unpkg.com/leaflet/dist/leaflet.js"></script>
  <script>
    const map = L.map('map').setView([21.90326817, -102.27382967], 13);

    L.tileLayer('https://{s}.tile.openstreetmap.org/{z}/{x}/{y}.png', {
      attribution: '&copy; <a href="https://www.openstreetmap.org/copyright">OpenStreetMap</a> contributors'
    }).addTo(map);

    const locations = <%- JSON.stringify(locations) %>;

    const latlngs = locations.map(loc => [parseFloat(loc.Altitud), parseFloat(loc.Longitud)]);

    if (latlngs.length > 0) {
      const polyline = L.polyline(latlngs, { color: 'blue' }).addTo(map);
      map.fitBounds(polyline.getBounds());

      // Marcar el punto de inicio
      const startMarker = L.marker([parseFloat(locations[0].Altitud), parseFloat(locations[0].Longitud)]).addTo(map);
      startMarker.bindPopup('<b>Inicio</b>').openPopup();

      // Añadir marcadores por defecto cada 5 timestamps
      for (let i = 15; i < locations.length; i += 15) {
        const marker = L.marker([parseFloat(locations[i].Altitud), parseFloat(locations[i].Longitud)]).addTo(map);
        marker.bindPopup(`<b>Timestamp:</b> ${new Date(locations[i].Fecha).toLocaleString()}`).openPopup();
      }
    }
  </script>
</body>
</html>
