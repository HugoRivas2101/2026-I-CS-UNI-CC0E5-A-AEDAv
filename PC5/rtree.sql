-- ============================================================
-- Índice Espacial (R-Tree via GiST) en PostgreSQL

-- PostgreSQL implementa R-Tree a traves de GiST (Generalized
-- Search Tree). Se activa con la extension PostGIS.
-- Soporta: ST_DWithin, ST_Intersects, ST_Contains, &&, etc.
-- NO es util para igualdad simple ni rangos numericos.
-- ============================================================

CREATE EXTENSION IF NOT EXISTS postgis;

DROP TABLE IF EXISTS puestos_postres;

CREATE TABLE puestos_postres (
    id          SERIAL PRIMARY KEY,
    nombre      VARCHAR(100) NOT NULL,
    postre      VARCHAR(80),
    ciudad      VARCHAR(60),
    region      VARCHAR(60),
    ubicacion   GEOMETRY(Point, 4326)  -- longitud, latitud en WGS84
);

-- Puestos de postres en distintas ciudades del Peru
-- POINT(longitud latitud)
INSERT INTO puestos_postres (nombre, postre, ciudad, region, ubicacion) VALUES
    ('Dulceria El Suspiro',   'Suspiro Limeño',    'Lima',      'Lima',         ST_SetSRID(ST_MakePoint(-77.0428, -12.0464), 4326)),
    ('La Picaronera del Jr',  'Picarones',         'Lima',      'Lima',         ST_SetSRID(ST_MakePoint(-77.0350, -12.0530), 4326)),
    ('Mazamorra Don Pepe',    'Mazamorra Morada',  'Lima',      'Lima',         ST_SetSRID(ST_MakePoint(-77.0500, -12.0600), 4326)),
    ('El Turrón de Barrios',  'Turrón de Doña Pepa','Lima',    'Lima',         ST_SetSRID(ST_MakePoint(-77.0290, -12.0410), 4326)),
    ('King Kong Original',    'King Kong',         'Chiclayo',  'Lambayeque',   ST_SetSRID(ST_MakePoint(-79.8400, -6.7741), 4326)),
    ('Tejas Iqueñas',         'Tejas de Ica',      'Ica',       'Ica',          ST_SetSRID(ST_MakePoint(-75.7286, -14.0678), 4326)),
    ('Bienmesabe Arequipeño', 'Bienmesabe',        'Arequipa',  'Arequipa',     ST_SetSRID(ST_MakePoint(-71.5375, -16.4090), 4326)),
    ('Queso Helado La Nieve', 'Queso Helado',      'Arequipa',  'Arequipa',     ST_SetSRID(ST_MakePoint(-71.5430, -16.4010), 4326)),
    ('Api de la Abuela',      'Api Morado',        'Puno',      'Puno',         ST_SetSRID(ST_MakePoint(-70.0219, -15.8402), 4326)),
    ('Ponche Andino',         'Ponche Cusqueño',   'Cusco',     'Cusco',        ST_SetSRID(ST_MakePoint(-71.9675, -13.5319), 4326)),
    ('Chapana Junineña',      'Chapana',           'Huancayo',  'Junin',        ST_SetSRID(ST_MakePoint(-75.2048, -12.0651), 4326)),
    ('Siete Sopas Andinas',   'Siete Sopas',       'Abancay',   'Apurimac',     ST_SetSRID(ST_MakePoint(-72.8811, -13.6340), 4326)),
    ('Dulces Trujillanos',    'Alfajor',           'Trujillo',  'La Libertad',  ST_SetSRID(ST_MakePoint(-79.0008,  -8.1120), 4326)),
    ('Ranfañote Express',     'Ranfañote',         'Lima',      'Lima',         ST_SetSRID(ST_MakePoint(-77.0480, -12.0390), 4326)),
    ('Frejol Colado Miraflores','Frejol Colado',   'Lima',      'Lima',         ST_SetSRID(ST_MakePoint(-77.0310, -12.1200), 4326));

-- Indice espacial GiST (implementa R-Tree internamente)
CREATE INDEX idx_spatial_ubicacion ON puestos_postres USING gist (ubicacion);

-- ============================================================
-- 1) ST_DWithin: puestos dentro de 5 km del centro de Lima
--    ST_DWithin en geografia usa metros; en geometry usa grados.
--    Usamos ST_Transform para convertir a metros (SRID 32718 = UTM Peru)
-- ============================================================
SELECT nombre, postre, ciudad
FROM puestos_postres
WHERE ST_DWithin(
    ubicacion::geography,
    ST_SetSRID(ST_MakePoint(-77.0428, -12.0464), 4326)::geography,
    5000  -- 5000 metros = 5 km
);

-- ============================================================
-- 2) ST_Distance: distancia desde el centro de Lima a cada puesto
-- ============================================================
SELECT nombre, ciudad,
       ROUND(ST_Distance(
           ubicacion::geography,
           ST_SetSRID(ST_MakePoint(-77.0428, -12.0464), 4326)::geography
       )::numeric / 1000, 2) AS distancia_km
FROM puestos_postres
ORDER BY distancia_km;

-- ============================================================
-- 3) ST_Intersects: puestos dentro de un rectangulo (bounding box)
--    que cubre aproximadamente Lima Metropolitana
-- ============================================================
SELECT nombre, postre, ciudad
FROM puestos_postres
WHERE ST_Intersects(
    ubicacion,
    ST_MakeEnvelope(-77.12, -12.20, -76.95, -11.95, 4326)
);

-- ============================================================
-- 4) && (bounding box overlap): mas rapido que ST_Intersects,
--    es la primera fase del filtro espacial del R-Tree
-- ============================================================
SELECT nombre, ciudad
FROM puestos_postres
WHERE ubicacion && ST_MakeEnvelope(-72.00, -17.00, -70.00, -15.00, 4326);
-- Cubre aproximadamente Arequipa y Puno

-- ============================================================
-- 5) ST_Contains: puestos dentro de un poligono personalizado
--    (rectangulo que cubre el sur del Peru)
-- ============================================================
SELECT nombre, postre, ciudad
FROM puestos_postres
WHERE ST_Contains(
    ST_MakeEnvelope(-77.00, -17.00, -69.00, -13.00, 4326),
    ubicacion
);

-- ============================================================
-- 6) Puesto mas cercano a Cusco (nearest neighbor)
-- ============================================================
SELECT nombre, postre, ciudad,
       ROUND(ST_Distance(
           ubicacion::geography,
           ST_SetSRID(ST_MakePoint(-71.9675, -13.5319), 4326)::geography
       )::numeric / 1000, 2) AS distancia_km
FROM puestos_postres
ORDER BY ubicacion <-> ST_SetSRID(ST_MakePoint(-71.9675, -13.5319), 4326)
LIMIT 3;
