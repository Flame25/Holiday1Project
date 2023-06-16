
# Holiday Project
Project using yolov5 combined with esp32-cam and firebase to create an automatic firefighter 

## Getting Started 

### Prerequisites
Requirements.txt located in 'Fire Detector and Distance Estimator/' Folder
```
pip install -r requirements.txt
```

## How to run

Example : 
```
python detect.py --save-txt --weights best.pt --conf 0.4 --source ../images/fire1.jpg --model_dist model@1535470106.json --weights_dist model@1535470106.h5
```
Full command : 

```
Usage - sources:
    $ python path/to/detect.py --weights yolov5s.pt --source 0              # webcam
                                                             img.jpg        # image
                                                             vid.mp4        # video
                                                             path/          # directory
                                                             path/*.jpg     # glob
                                                             'https://youtu.be/Zgi9g1ksQHc'  # YouTube
                                                             'rtsp://example.com/media.mp4'  # RTSP, RTMP, HTTP stream

Usage - formats:
    $ python path/to/detect.py --weights yolov5s.pt                 # PyTorch
                                         yolov5s.torchscript        # TorchScript
                                         yolov5s.onnx               # ONNX Runtime or OpenCV DNN with --dnn
                                         yolov5s.xml                # OpenVINO
                                         yolov5s.engine             # TensorRT
                                         yolov5s.mlmodel            # CoreML (macOS-only)
                                         yolov5s_saved_model        # TensorFlow SavedModel
                                         yolov5s.pb                 # TensorFlow GraphDef
                                         yolov5s.tflite             # TensorFlow Lite
                                         yolov5s_edgetpu.tflite     # TensorFlow Edge TPU
```

## Demo
![Example 1](https://github.com/Flame25/Holiday1Project/blob/main/Fire%20Detector%20and%20Distance%20Estimator/runs/detect/exp4/fire1.jpg)

