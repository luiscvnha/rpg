# Random Password Generator
Using `/dev/urandom` for the randomness

## Installation
```
$ make
```

## How to use
```
$ ./rpg <flags> <password length>
```

flags | meaning
---|---
-l | use lowercase letters
-u | use uppercase letters
-n | use numbers
-s | use special characters

flags can be combined

### Example
```
$ ./rpg -lun 20
SeChHb8l633UbyxVEMri
```
