function p = show_geoVolume(hdf5FileName, datasetName)
%SHOW_GEOVOLUME Summary of this function goes here
%   Detailed explanation goes here

    data = hdf5read(hdf5FileName,datasetName);
    data = permute(data,[3 2 1]); %Matlab rows are inverted relative to hdf5 format
    
    [x,y,z] = meshgrid(1:size(data,1), 1:size(data,2), 1:size(data,3));
    figure()
    p = patch(isosurface(x,y,z,data,1/2));
    isonormals(x,y,z,data,p)
    p.FaceColor = [54,195,224]./255;
    p.EdgeColor = 'none';
    daspect([1,1,1])
    view(3); axis tight
    camlight
    lighting gouraud
end

