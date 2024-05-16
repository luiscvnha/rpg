# Random Password Generator

Using `/dev/urandom` for the randomness.

## Installation
```
make
```

## How to use
```
./rpg <options> <password length>
```

options | meaning
---|---
-l | use lowercase letters
-u | use uppercase letters
-n | use numbers
-s | use special characters

### Example
```
./rpg -lun 16
```
