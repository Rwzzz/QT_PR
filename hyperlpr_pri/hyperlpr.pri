HEADERS +=\
        $$PWD/include/CNNRecognizer.h \
        $$PWD/include/FastDeskew.h \
        $$PWD/include/FineMapping.h \
        $$PWD/include/Pipeline.h \
        $$PWD/include/PlateDetection.h \
        $$PWD/include/PlateInfo.h \
        $$PWD/include/PlateSegmentation.h \
        $$PWD/include/Recognizer.h \
        $$PWD/include/SegmentationFreeRecognizer.h \
        $$PWD/include/niBlackThreshold.h \
        $$PWD/src/util.h \
        $$PWD/widget.h
		
		
SOURCES += \
        $$PWD/src/CNNRecognizer.cpp \
        $$PWD/src/FastDeskew.cpp \
        $$PWD/src/FineMapping.cpp \
        $$PWD/src/Pipeline.cpp \
        $$PWD/src/PlateDetection.cpp \
        $$PWD/src/PlateSegmentation.cpp \
        $$PWD/src/Recognizer.cpp \
        $$PWD/src/SegmentationFreeRecognizer.cpp \
		
data.files += model/*.prototxt
data.files += model/*.caffemodel
data.files += model/*.xml
data.path = /assets/model
INSTALLS += data
		
DISTFILES += \
        $$PWD/model/CharacterRecognization.caffemodel \
        $$PWD/model/CharacterRecognization.prototxt \
        $$PWD/model/HorizonalFinemapping.caffemodel \
        $$PWD/model/HorizonalFinemapping.prototxt \
        $$PWD/model/SegmenationFree-Inception.caffemodel \
        $$PWD/model/SegmenationFree-Inception.prototxt \
        $$PWD/model/Segmentation.caffemodel \
        $$PWD/model/Segmentation.prototxt \
        $$PWD/model/cascade.xml
