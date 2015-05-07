extern crate image;
extern crate nalgebra;
extern crate num;

mod regressor;

use image::GenericImage;
use nalgebra::{DMat,Indexable};
use std::path::Path;

use regressor::Regressor;

pub static NUM_REGRESSOR: usize = 10;
pub static NUM_FERN: usize = 500;
pub static NUM_INITIAL: usize = 20;
pub static NUM_CANDIDATE_PIXEL: usize = 400;
pub static NUM_FERN_PIXEL: usize = 5;

pub struct Alignment {
    image_list: Vec<DMat<u8>>,
    bounding_box_list: Vec<BoundingBox>,
    shape_list: Vec<DMat<f64>>,
    regressor_list: Vec<Regressor>,
    mean_shape: DMat<f64>,
    num_landmark: usize,
    num_regressor: usize,
    trained: bool,
}

pub struct BoundingBox {
    x: f64,
    y: f64,
    width: f64,
    height: f64,
    center_x: f64,
    center_y: f64,
}

pub struct Point {
    x: f64,
    y: f64,
}


impl Alignment {
    pub fn new() -> Alignment {
        return Alignment{
            image_list: Vec::new(),
            bounding_box_list: Vec::new(),
            shape_list: Vec::new(),
            regressor_list: Vec::new(),
            mean_shape: DMat::new_zeros(1,2),
            num_landmark: 0,
            num_regressor: 0,
            trained: false,
        };
    }
    pub fn load_model(path: &str) -> Alignment {
        println!("Loading model from: {}", path);
        let mut alignment = Alignment::new();
        // TODO load the model
        alignment.trained = true;
        return alignment;
    }
    fn read_image(path: &str) -> DMat<u8> {
        let img = image::open(&Path::new(path)).unwrap().to_luma();
        let img_height = img.height();
        let img_width = img.width();
        let img_mat: DMat<u8> = DMat::from_row_vec(img_height as usize, img_width as usize, &img.into_raw());
        println!("hehe => {}, dimension: height => {}, width => {}", path, img_height, img_width);
        img_mat
    }
    pub fn feed_sample(&mut self, path: &str, bounding_box: BoundingBox, shape: Vec<Point>) {
        if self.num_landmark != 0 && self.num_landmark != shape.len() {
            panic!("the shape size doesn't match");
        }
        self.num_landmark = shape.len();
        self.image_list.push(Alignment::read_image(path));
        self.bounding_box_list.push(bounding_box);
        //self.shape_list.push(shape);
        let mut shape_mat: DMat<f64> = DMat::new_zeros(shape.len(), 2);
        for idx in 0..shape.len() {
            let point = &shape[idx];
            //shape_mat.set((idx, 0), point.x);
            //shape_mat.set((idx, 1), point.y);
            shape_mat[(idx,0)] = point.x;
            shape_mat[(idx,1)] = point.x;
        }
        self.shape_list.push(shape_mat);
    }
    pub fn save_model(&self, path: &str) {
        if !self.trained {
            panic!("the model is not trained!");
        }
        println!("Saving the model to: {}", path);
    }

    pub fn train(&mut self, num_regressor: usize, num_fern: usize, 
                num_candidate_pixel: usize, num_fern_pixel: usize, num_initial: usize) {
        if self.num_landmark == 0 {
            panic!("the size of shape is zero!");
        }
        if !self.trained {
            panic!("there is no trained model loaded!");
        }
        // TODO: train the model
        let len: usize = self.shape_list.len();
        assert!(len != 0);
        for i in 0..len {
            for j in 0..num_initial {
                let mut index: usize = 0;
                while index == i {
                    //index = usize::rand() % len;
                }
            }
        }

        self.mean_shape = regressor::get_mean_shape(&self.shape_list, &self.bounding_box_list);
        self.num_regressor = num_regressor;
        self.regressor_list.reserve(num_regressor);
        for idx in 0..num_regressor {
            self.regressor_list.push(Regressor::new());
        }
        
        self.trained = true;
    }

    pub fn predict(&self, path: &str, bounding_box: BoundingBox) -> Vec<Point> {
        if self.num_landmark == 0 {
            panic!("the size of shape is zero!");
        }
        if !self.trained {
            panic!("there is no trained model loaded!");
        }
        // TODO: begin to predict
        return Vec::new();
    }
}

impl BoundingBox {
    pub fn new(x: f64, y: f64, w: f64, h: f64) -> BoundingBox {
        return BoundingBox {
            x: x,
            y: x,
            width: w,
            height: h,
            center_x: x + w / 2.0f64,
            center_y: y + h / 2.0f64,
        };
    }
}

impl Point {
    pub fn new(x: f64, y: f64) -> Point {
        return Point{
            x: x,
            y: y,
        };
    }
}
