-- ============================================================
-- B-Tree Index: igualdad, rangos, BETWEEN, IN, ORDER BY, LIKE

-- El BTree mantiene las claves ordenadas -> soporta todo tipo
-- de comparacion de orden
-- ============================================================

DROP TABLE IF EXISTS postres_peruanos;

CREATE TABLE postres_peruanos (
    id          SERIAL PRIMARY KEY,
    nombre      VARCHAR(100) NOT NULL,
    region      VARCHAR(60),
    tipo        VARCHAR(50),
    calorias    INT,
    precio_sol  DECIMAL(8,2),
    sin_gluten  BOOLEAN
);

INSERT INTO postres_peruanos (nombre, region, tipo, calorias, precio_sol, sin_gluten) VALUES
    ('Suspiro Limeño',      'Lima',         'Crema',    430,  8.50, true),
    ('Picarones',           'Lima',         'Frito',    320,  4.00, false),
    ('Mazamorra Morada',    'Lima',         'Mazamorra',210,  5.00, true),
    ('Arroz con Leche',     'Lima',         'Arroz',    280,  4.50, false),
    ('Turrón de Doña Pepa','Lima',         'Turrón',   490,  6.00, false),
    ('King Kong',           'Lambayeque',   'Alfajor',  520, 12.00, false),
    ('Dulce de Guindones',  'Lambayeque',   'Dulce',    180,  3.50, true),
    ('Tejas de Ica',        'Ica',          'Dulce',    260,  7.00, true),
    ('Frejol Colado',       'Lima',         'Dulce',    350,  5.50, true),
    ('Chancay',             'Lima',         'Pan',      310,  2.50, false),
    ('Bienmesabe',          'Arequipa',     'Crema',    390,  6.50, true),
    ('Queso Helado',        'Arequipa',     'Helado',   220,  5.00, true),
    ('Ranfañote',           'Lima',         'Dulce',    410,  4.00, false),
    ('Chapana',             'Junin',        'Dulce',    270,  3.00, true),
    ('Api Morado',          'Puno',         'Bebida',   160,  3.50, true),
    ('Ponche Cusqueño',     'Cusco',        'Bebida',   190,  4.50, true),
    ('Cachanga',            'Costa',        'Frito',    340,  2.00, false),
    ('Buñuelo',             'Lima',         'Frito',    300,  2.50, false),
    ('Maicena',             'Lima',         'Crema',    240,  4.00, true),
    ('Siete Sopas',         'Apurimac',     'Mazamorra',200,  3.50, true);

-- "USING btree" es opcional, es el tipo por defecto.
CREATE INDEX idx_postres_precio    ON postres_peruanos USING btree (precio_sol);
CREATE INDEX idx_postres_nombre    ON postres_peruanos (nombre);
CREATE INDEX idx_postres_calorias  ON postres_peruanos (calorias);

-- ============================================================
-- 1) Igualdad: todos los postres de una region
-- ============================================================
SELECT nombre, tipo, precio_sol FROM postres_peruanos WHERE region = 'Lima';

-- ============================================================
-- 2) Rango: BTree puede buscar por rango, Hash no puede
-- ============================================================
SELECT nombre, calorias FROM postres_peruanos WHERE calorias > 350 ORDER BY calorias;

-- ============================================================
-- 3) BETWEEN: postres en rango de precio accesible
-- ============================================================
SELECT nombre, region, precio_sol
FROM postres_peruanos
WHERE precio_sol BETWEEN 3.00 AND 6.00
ORDER BY precio_sol;

-- ============================================================
-- 4) IN: postres de regiones especificas
-- ============================================================
SELECT nombre, region, tipo
FROM postres_peruanos
WHERE region IN ('Arequipa', 'Cusco', 'Puno');

-- ============================================================
-- 5) ORDER BY: el indice ya esta ordenado, evita el sort
-- ============================================================
SELECT nombre, calorias FROM postres_peruanos ORDER BY calorias DESC LIMIT 5;

-- ============================================================
-- 6) LIKE con prefijo fijo: BTree puede usar el indice
--    LIKE '%abc' (sufijo) NO usa el indice
-- ============================================================
SELECT nombre, region FROM postres_peruanos WHERE nombre LIKE 'M%';
