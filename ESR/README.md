# To Compile ESR

## Requirement

1. OpenCV: for image reading and writing, and some calculation
```
sudo apt-get install libopencv-dev	# for Ubuntu
emerge -av opencv			# for Gentoo
```
2. GCC with C++ 11 support


## Compile

```
make
```

## Components

After `make`, there will be three individual components in the same directory:

1. esr-train => train the model
2. err-detect => calculate the error using the images from test set
3. esr-predict-server => the backend server for the web page demo
