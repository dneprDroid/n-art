//
//  ViewController.swift
//  NArt-ios
//
//  Created by user on 3/24/17.
//  Copyright © 2017 alex. All rights reserved.
//

import UIKit

class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
    }
    
    override func viewDidAppear(_ animated: Bool) {
        super.viewDidAppear(animated)
        
        let picker = UIImagePickerController()
        picker.delegate = self
        picker.allowsEditing = false
        picker.sourceType = .photoLibrary
        
        self.present(picker, animated: true) {
            
        }

    }

}

extension ViewController: UIImagePickerControllerDelegate, UINavigationControllerDelegate {
    
    func imagePickerController(_ picker: UIImagePickerController, didFinishPickingMediaWithInfo info: [String : Any]) {
        if let image = info[UIImagePickerControllerOriginalImage] as? UIImage {
            dismiss(animated: true, completion: nil)

            let art = NArt()
            print("image size: \( image.size )")
            art?.styleImage(image)
        }
    }
}
