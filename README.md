# ML-project
A repo for EECS 349 Machine Learning project for Northwestern University 

In this project, we try to use AAM(Active Appearance Model) and ESR(Explicit Shape Regression)[1] to train the face alignment model and use this trained model to do the face alignment job.



## Dataset
We use the following dataset which are commonly used by face alignment research:

1. IBUG( http://ibug.doc.ic.ac.uk/resources/facial-point-annotations/ )[2,3]
2. Helen( http://www.ifp.illinois.edu/~vuongle2/helen/ )[4]

## Preprocess

1. Convert all the raw image into grayscale image
2. Use the face detector provided by OpenCV to pre-compute the bouding box for alignment

## Collaborator
Yang Yang, Haomin Hu, Can Wang, Lijun Tang


## Reference
[1]: Cao, Xudong, et al. "Face alignment by explicit shape regression." International Journal of Computer Vision 107.2 (2014): 177-190.
[2]: C. Sagonas, G. Tzimiropoulos, S. Zafeiriou, M. Pantic. A semi-automatic methodology for facial landmark annotation. Proceedings of IEEE Int’l Conf. Computer Vision and Pattern Recognition (CVPR-W’13), 5th Workshop on Analysis and Modeling of Faces and Gestures (AMFG '13). Oregon, USA, June 2013.
[3]: C. Sagonas, G. Tzimiropoulos, S. Zafeiriou, M. Pantic. 300 Faces in-the-Wild Challenge: The first facial landmark localization Challenge. Proceedings of IEEE Int’l Conf. on Computer Vision (ICCV-W 2013), 300 Faces in-the-Wild Challenge (300-W). Sydney, Australia, December 2013. 
[4]: Le, Vuong, et al. "Interactive facial feature localization." Computer Vision–ECCV 2012. Springer Berlin Heidelberg, 2012. 679-692.
