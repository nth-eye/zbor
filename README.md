# zbor

Small C++17 CBOR stream codec. No dynamic memory allocation, all items are encoded/parsed on-the-fly in a given buffer. Decoding can be done manually with `zbor::decode()` or using `zbor::Seq` wrapper in range-based for loop. Range safely stops at anything invalid, but doesn't provide info about failure. To get exact `zbor::Err` you need to decode and check manually every item.

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

for (auto it : zbor::Seq{example, sizeof(example)}) {
    switch (it.type) {
        case zbor::TYPE_UINT:
            printf("got uint %lu \n", it.uint); 
        break;
        case zbor::TYPE_TEXT:
            printf("got text \"%.*s\" \n", int(it.str.len), it.str.txt); 
        break;
        case zbor::TYPE_ARRAY:
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

zbor::Obj obj;
zbor::Err err;
auto ptr = illformed;
auto end = illformed + sizeof(illformed);

while (1) {

    std::tie(obj, err, ptr) = zbor::decode(ptr, end);
    
    if (err != zbor::ERR_OK) {
        printf("got error, %d -> %s", err, zbor::str_err(err));
        break;
    }
    switch (obj.type) {
        case zbor::TYPE_MAP:
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

```cpp
```

## TODO

- [ ] encoder tests
- [ ] examples in readme

[1]: https://github.com/nth-eye/utl
