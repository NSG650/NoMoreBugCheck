# NoMoreBugCheck

Prevent Windows from BSODing no matter what happens!

# Warning

Windows/Drivers calls a BugCheck when something is severely wrong.
By preventing the system from being taken down it can lead to corruption of memory or data.
So dont use this if you want to work on a project and dont want Windows to BSOD.
Theres a good chance you would corrupt memory or data.
I am not responsible for the damage done to the system.
This was done only for fun.

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

# Note

- If you want to revert the changes just unload the driver by running

```
sc stop NoMoreBugCheck
```

- The system can hang if the problem was severe.