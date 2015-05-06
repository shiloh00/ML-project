extern crate image;
extern crate nalgebra;

mod regressor;

use image::GenericImage;
use nalgebra::{DMat};
use std::path::Path;

use regressor::Regressor;

pub struct Alignment {
    image_list: Vec<DMat<u8>>,
    bounding_box_list: Vec<BoundingBox>,
    shape_list: Vec<Vec<Point>>,
    regressor_list: Vec<Regressor>,
    mean_shape: Vec<Point>,
//    num_regressor: usize,
//    num_landmark: usize,
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
            mean_shape: Vec::new(),
//            num_regressor: 10,
//            num_landmark: 0,
        };
    }
    pub fn load_model(path: &str) -> Alignment {
        println!("Loading model from: {}", path);
        let alignment = Alignment::new();
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
        self.image_list.push(Alignment::read_image(path));
        self.bounding_box_list.push(bounding_box);
        self.shape_list.push(shape);
    }
    pub fn save_model(&self, path: &str) {
        println!("Saving the model to: {}", path);
    }

    pub fn train(&self, num_regressor: usize, num_fern_per_regressor: usize, 
                 num_cand_pix: usize, num_fern_pix: usize, num_initial: usize) {
    }

    pub fn predict(&self, path: &str, bounding_box: BoundingBox, num_initial: usize) -> Vec<Point> {
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
