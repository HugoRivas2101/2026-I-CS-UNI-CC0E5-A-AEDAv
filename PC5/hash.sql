-- ============================================================
-- Hash Index: SOLO igualdad (=)

-- Usa una funcion hash para mapear claves a buckets.
-- MAS rapido que BTree en igualdad exacta, pero NO soporta
-- rangos, ORDER BY, BETWEEN ni LIKE.
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

-- Hash index: optimo para busquedas por igualdad exacta
CREATE INDEX idx_hash_nombre ON postres_peruanos USING hash (nombre);
CREATE INDEX idx_hash_region ON postres_peruanos USING hash (region);
CREATE INDEX idx_hash_tipo   ON postres_peruanos USING hash (tipo);

-- ============================================================
-- 1) Igualdad exacta por nombre:
--    O(1) promedio vs O(log n) de BTree
-- ============================================================
SELECT * FROM postres_peruanos WHERE nombre = 'King Kong';

-- ============================================================
-- 2) Igualdad exacta por region
-- ============================================================
SELECT nombre, precio_sol FROM postres_peruanos WHERE region = 'Arequipa';

-- ============================================================
-- 3) Igualdad exacta por tipo
-- ============================================================
SELECT nombre, region FROM postres_peruanos WHERE tipo = 'Crema';

-- ============================================================
-- 4) Busqueda exacta por boolean
-- ============================================================
SELECT nombre, region FROM postres_peruanos WHERE sin_gluten = true;