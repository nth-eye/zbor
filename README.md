# zbor

Small C++17 CBOR stream codec. No dynamic memory allocation, all items are encoded/parsed on-the-fly in a given buffer. Decoding can be done manually with `zbor::decode()` or using `zbor::seq_t` wrapper in range-based for loop. Range safely stops at anything invalid, but doesn't provide info about failure. To get exact `zbor::err_t` you need to decode and check manually every item.

__Half-float support included with [`utl::`][1]!__

## Examples

### Decode

#### With range-based for loop

```cpp
const uint8_t example[] = { 
    0x01, // 1
    0x64, 0x5a, 0x42, 0x4f, 0x52, // "ZBOR"
    0x83, 0x01, 0x02, 0x03, // [1, 2, 3]
};

for (auto it : zbor::seq_t{example, sizeof(example)}) {
    switch (it.type) {
        case zbor::type_uint:
            printf("got uint %lu \n", it.uint); 
        break;
        case zbor::type_text:
            printf("got text \"%.*s\" \n", int(it.str.len), it.str.txt); 
        break;
        case zbor::type_array:
            if (it.arr.indef())
                printf("got indefinite array... \n");
            else
                printf("got array of size %lu... \n", it.arr.size());
            for (auto arr_it : it.arr)
                printf("\t %s \n", zbor::str_type(arr_it.type));
        break;
        default:
            printf("got unawaited \n");
    }
}
```

#### Manually

```cpp
const uint8_t illformed[] = {
    0xbf, 0x81, 0x00, 0xf5, 0xff, // {_ [0]: true}
    0xfe, // <invalid> - AI 30 is reserved
};

zbor::obj_t obj;
zbor::err_t err;
auto ptr = illformed;
auto end = illformed + sizeof(illformed);

while (1) {

    std::tie(obj, err, ptr) = zbor::decode(ptr, end);
    
    if (err != zbor::err_ok) {
        printf("got error, %d -> %s", err, zbor::str_err(err));
        break;
    }
    switch (obj.type) {
        case zbor::type_map:
            if (obj.map.indef())
                printf("got indefinite map... \n");
            else
                printf("got map of size %lu... \n", obj.map.size());
            for (auto [key, val] : obj.map)
                printf("\t %s: %s\n", zbor::str_type(key.type), zbor::str_type(val.type));
        break;
        default:
            printf("got unawaited \n");
    }
}
```

### Encode

#### With `Codec` wrapper

```cpp
zbor::Codec<64> msg;

const uint8_t data[] = {0x44, 0x45};

msg.encode_arr(3);                  // start fixed size array
msg.encode(-1);                     // negative int
msg.encode(1);                      // positive int
msg.encode(1u);                     // explicitly positive

msg.encode_map(1);                  // start fixed size map
msg.encode("text");                 // text string
msg.encode({data, sizeof(data)});   // byte string

msg.encode_tag(69);                 // tag number, next object will be content
msg.encode_indef_arr();             // start indefinite size array (previously tagged)
msg.encode(true);                   // simple bool
msg.encode(zbor::prim_null);        // simple null
msg.encode(zbor::prim_t(42));         // another valid simple (primitive)
msg.encode_break();                 // break, end of indefinite array

msg.encode_indef_map();             // start indefinite size map
msg.encode(42.0f);                  // float32, will be compressed to half-float if possible
msg.encode(42.0);                   // float64, will be compressed to half-float if possible
msg.encode_break();                 // break, end of indefinite map

msg.encode_indef_txt();             // start indefinite size text string made from separate chunks
msg.encode("Hello");                // first chunk
msg.encode("World");                // second chunk
msg.encode_break();                 // break, end of indefinite text string

some_c_style_handler(msg.data(), msg.size());

zbor::log_seq(msg); // expected result below

msg.clear(); // to reuse codec
```
```
+-----------HEX-----------+
| 83 20 01 01 a1 64 74 65  78 74 42 44 45 d8 45 9f  |. ...dtextBDE.E.|
| f5 f6 f8 2a ff bf f9 51  40 f9 51 40 ff 7f 65 48  |...*...Q@.Q@..eH|
| 65 6c 6c 6f 65 57 6f 72  6c 64 ff                 |elloeWorld......|
+--------DIAGNOSTIC-------+
| 1) [-1, 1, 1]
| 2) {"text": h'4445'}
| 3) 69([_ true, null, simple(42)])
| 4) {_ 42.0: 42.0}
| 5) (_ "Hello", "World")
+-------------------------+
```

#### With provided buffer (e.g. dynamically allocated)

```cpp
auto buf = (uint8_t*) malloc(10);   // let's assume someone still uses malloc...
auto msg = zbor::Buf{buf, 10};      // exactly as with Codec after this
auto err = msg.encode("too long string");

if (err == zbor::err_ok)
    zbor::log_seq(msg);
else
    printf("error: %d -> %s", err, zbor::str_err(err));
```

## TODO

- [ ] encoder tests
- [x] examples in readme
- [ ] maybe try to constexpr whole library ?
- [ ] review naming conventions
- [ ] forbid implicit const char* conversion to bool

[1]: https://github.com/nth-eye/utl
