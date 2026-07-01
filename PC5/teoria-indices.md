# Índices en PostgreSQL: BTree, Hash y Espacial (RTree)

## ¿Qué es un índice?

Una estructura de datos auxiliar que PostgreSQL mantiene junto a la tabla para
acelerar las búsquedas. Sin índice, PostgreSQL hace un **sequential scan** —
revisa cada fila una por una. Con índice, salta directamente a las filas relevantes.

El costo: espacio en disco adicional y mayor tiempo en INSERT/UPDATE/DELETE
(porque hay que actualizar el índice). Vale la pena cuando las lecturas son frecuentes.

---

## 1. BTree (Balanced Tree)

### Estructura

Árbol balanceado donde las claves están ordenadas. Cada nodo interno tiene
claves y punteros a hijos. Las hojas apuntan a los registros reales en el heap.

```
             [M]
           /     \
       [D, H]   [R, U]
      /  |  \     ...
  [A,C][E,G][I,L]       ← hojas con punteros a filas
```

### Cómo funciona

Al buscar `WHERE precio = 50`, PostgreSQL navega desde la raíz comparando
claves en cada nivel hasta llegar a la hoja correcta: **O(log n)**.

Para un rango `WHERE precio BETWEEN 30 AND 80`, encuentra el primer valor
y recorre las hojas secuencialmente hasta el último: **O(log n + k)**.

### Operaciones soportadas

| Operador | ¿Usa índice? |
|----------|-------------|
| `=`      | ✅ |
| `<`, `>`, `<=`, `>=` | ✅ |
| `BETWEEN` | ✅ |
| `IN` | ✅ |
| `ORDER BY` | ✅ (ya está ordenado) |
| `LIKE 'abc%'` | ✅ (solo prefijos) |
| `LIKE '%abc'` | ❌ (sufijos no) |
| Operaciones geométricas | ❌ |

### Cuándo usarlo

- Caso general — es el índice más versátil
- Columnas con alta cardinalidad (muchos valores distintos)
- Columnas usadas en rangos, ORDER BY o JOIN
- **Es el índice por defecto** (`CREATE INDEX` sin USING)

```sql
CREATE INDEX idx_precio ON postres USING btree (precio);
CREATE INDEX idx_precio ON postres (precio); -- equivalente
```

---

## 2. Hash

### Estructura

Una tabla hash donde cada clave pasa por una función hash que produce un
número de bucket. Todos los registros con el mismo hash van al mismo bucket.

```
nombre → hash('King Kong') → bucket 42 → [punteros a filas]
nombre → hash('Suspiro')   → bucket 17 → [punteros a filas]
```

### Cómo funciona

Al buscar `WHERE nombre = 'King Kong'`, aplica hash al valor y va directo
al bucket: **O(1) promedio**. Más rápido que BTree para igualdad exacta.

Pero los buckets no tienen orden entre sí — hash('A') puede estar en el
bucket 99 y hash('B') en el bucket 3. Por eso **no puede hacer rangos**.

### Operaciones soportadas

| Operador | ¿Usa índice? |
|----------|-------------|
| `=`      | ✅ (O(1) promedio) |
| `<`, `>`, `<=`, `>=` | ❌ |
| `BETWEEN` | ❌ |
| `ORDER BY` | ❌ |
| `LIKE` | ❌ |
| `IN` | ❌ |

### Cuándo usarlo

- Columnas donde **solo** se hacen búsquedas por igualdad exacta
- Uniones por clave foránea (`JOIN ON a.id = b.id`)
- Cuando la velocidad de igualdad importa más que la flexibilidad

```sql
CREATE INDEX idx_nombre ON postres USING hash (nombre);
```

> **Nota:** Antes de PostgreSQL 10, los índices Hash no eran crash-safe
> (no usaban WAL). Desde la versión 10 son seguros y recomendados.

---

## 3. Espacial — RTree (via GiST)

### Estructura

PostgreSQL implementa el R-Tree a través de **GiST** (Generalized Search Tree).
El R-Tree organiza objetos geográficos por sus **bounding boxes** (rectángulos
mínimos que los contienen) en una jerarquía de niveles.

```
Nivel 0 (raíz):   [bounding box de todo el Peru]
Nivel 1:          [Lima + alrededores] [Sur del Peru] [Norte del Peru]
Nivel 2 (hojas):  [punto Miraflores] [punto San Isidro] [punto Arequipa] ...
```

### Cómo funciona

Al buscar "puestos a menos de 5 km de Lima", el R-Tree descarta ramas enteras
cuyo bounding box no intersecta con el radio buscado. Solo navega las ramas
que pueden contener resultados. **Evita revisar todos los puntos**.

### Operaciones soportadas

| Función / Operador | Descripción |
|--------------------|-------------|
| `ST_DWithin(a, b, d)` | ¿Está `a` dentro de distancia `d` de `b`? |
| `ST_Intersects(a, b)` | ¿Se intersectan `a` y `b`? |
| `ST_Contains(a, b)` | ¿Contiene `a` completamente a `b`? |
| `ST_Distance(a, b)` | Distancia entre `a` y `b` |
| `a && b` | ¿Los bounding boxes se superponen? (más rápido) |
| `a <-> b` | Distancia (para ORDER BY + LIMIT = nearest neighbor) |

### Cuándo usarlo

- Datos geográficos: puntos, líneas, polígonos
- Búsquedas de proximidad ("restaurantes cercanos")
- Consultas de intersección ("calles que cruzan este distrito")
- Requiere la extensión **PostGIS**

```sql
CREATE EXTENSION IF NOT EXISTS postgis;
CREATE INDEX idx_ubicacion ON puestos USING gist (ubicacion);
```

---

## Comparación general

| Criterio | BTree | Hash | RTree (GiST) |
|----------|-------|------|--------------|
| Estructura | Árbol balanceado | Tabla hash | Árbol de bounding boxes |
| Igualdad `=` | ✅ O(log n) | ✅ O(1) | ❌ no aplica |
| Rangos `<`, `>` | ✅ | ❌ | ❌ |
| ORDER BY | ✅ | ❌ | ❌ |
| Espacial | ❌ | ❌ | ✅ |
| Tipo de dato | Cualquiera ordenable | Cualquiera | Geometría |
| Índice por defecto | ✅ | ❌ | ❌ |
| Necesita extensión | ❌ | ❌ | ✅ PostGIS |

---