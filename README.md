# NoMoreBugCheck

Prevent Windows from BSODing no matter what happens!

# Warning

Using this is the equivalent of disabling the circuit breaker in your house.
Windows BSODs whenever a serious problem occurs to prevent memory/data corruption.
Although your computer won't blow up you could corrupt memory/data present on the system.
This was done only for fun and should not be used for any serious purposes.
I am not responsible for loss of any data or damage to the system.

# How to use?

1. Enable test signing

```
bcdedit /set testsigning on
```

2. Create a service using SC

```
sc create NoMoreBugCheck binPath=C:\where\ever\the\driver\is\NoMoreBugCheck.sys type=kernel start=manual
```

3. Run it!

```
sc start NoMoreBugCheck
```

# Demo

https://user-images.githubusercontent.com/51860844/146301386-68e4b170-89c1-441e-97b0-c5bdd2b16ed8.mp4

# Note

- If you want to revert the changes just unload the driver by running
```
sc stop NoMoreBugCheck
```

- The system can hang if the problem was severe.
