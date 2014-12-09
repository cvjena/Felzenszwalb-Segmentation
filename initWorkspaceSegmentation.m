function initWorkspaceSegmentation

    %% setup paths in use-specific manner
    
    if  strcmp( getenv('USER'), 'freytag')
        % dependencies go here      
    else
        fprintf('Unknown user %s and unknown default settings', getenv('USER') ); 
    end

    %% add paths
    
    % add main path
    b_recursive            = false; 
    b_overwrite            = true;
    s_pathMain             = fullfile(pwd);
    addPathSafely ( s_pathMain, b_recursive, b_overwrite )
    clear ( 's_pathMain' );      
    
    
    
    %% clean up
        
end


function addPathSafely ( s_path, b_recursive, b_overwrite )
    if ( ~isempty(strfind(path, [s_path , pathsep])) )
        if ( b_overwrite )
            if ( b_recursive )
                rmpath( genpath( s_path ) );
            else
                rmpath( s_path );
            end
        else
            fprintf('InitPatchDiscovery - %s already in your path but overwriting de-activated.\n', s_path);
            return;
        end
    end
    
    if ( b_recursive )
        addpath( genpath( s_path ) );
    else
        addpath( s_path );
    end
end
