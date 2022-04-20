# zbor

Small C++14 Obj encoder/decoder library without dynamic memory allocation. Obj tokens can be allocated from static object pool templated by maximum number of tokens.

## Examples

### Array

```cpp
zbor::Obj el_0 = 1;
zbor::Obj el_1 = -42;

zbor::Array as_arr;

as_arr.push(&el_0);
as_arr.push(&el_1);

zbor::Obj as_obj = zbor::Array();

as_obj.arr.push(&el_0);
as_obj.arr.push(&el_1);
```

### Map

```cpp
zbor::Obj key = {"key", 3};
zbor::Obj val = {(uint8_t*) "\xde\xad\xc0\xde", 4};

zbor::Map as_map;

as_map.push(&key, &val);

zbor::Obj as_obj = zbor::Map();

as_obj.map.push(&key, &val);
```

### Pool

```cpp
zbor::Pool<4> pool;
zbor::Array arr;
zbor::Obj *ptr;

arr.push(pool.make(true));
arr.push(pool.make(false));
arr.push(pool.make(Prim(69)));

ptr = pool.make(PRIM_NULL);
pool.free(ptr);

ptr = pool.make(arr);
pool.free(ptr); // NOTE: Doesn't automatically free elements

ptr = pool.make(arr);
for (auto it : ptr->arr) // Free elements (non recursive)
    pool.free(it);
pool.free(ptr);
```

### Encoding

```cpp
zbor::Pool<1> pool;
zbor::Encoder<9> enc;

Err err = enc.encode(pool.make(0xfffffffffffffffful));

if (err != zbor::NO_ERR) {
    printf("encode failure: %d \n", err);
    return;
}

for (size_t i = 0; i < enc.size(); ++i)
    printf("%02x ", enc[i]);
printf("\n");

some_c_style_fn(enc.data(), enc.size());
```

### Decoding

```cpp
```
