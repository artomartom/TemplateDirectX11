
[CmdletBinding()]
param (
    [ValidateNotNullOrEmpty()]
    [string]
    $Config
)
  
begin {}
  
process {
    Import-Module 'F:\Dev\Projects\PowerShell\DXUtil.psm1';
        
    
    Class Shader {
        
        Shader([string]$NewEntryPoint,
            [string]$NewProfile,
            [string]$NewOutName ) {

            $this.EntryPoint = $NewEntryPoint;
            $this.Profile = $NewProfile;
            $this.OutName = $NewOutName;
           
        }

        [string]$EntryPoint;
        [string]$Profile;
        [string]$OutName;
    }

    $Shaders = @(
        [Shader]::new('VSMain', 'vs_5_0', 'Vertex.so' ),
        [Shader]::new('PSMain', 'ps_5_0', 'Pixel.so' ) 
    );
    
    $Configs = @(  'Debug', 'Release'    ) ;
          
    foreach ($eachConfig in $Configs) {
        foreach ($eachShader in $Shaders) {
            Invoke-fxcCompiler  -Path  "$($PSScriptRoot)/Source//Shader//Source//main.hlsl" -EntryPoint    $eachShader.EntryPoint -Profile $eachShader.Profile      `
                -OutputType   Object  -Destination   "$($PSScriptRoot)/Build//$($eachConfig)//$($eachShader.OutName)" -Config $eachConfig -VarName $eachShader.VarName ;
            if ($LASTEXITCODE -ne 0 ) {
                return $LASTEXITCODE;
            };
        }
    }
    Remove-Module 'DXUtil'   ;


    Copy-Item "$($PSScriptRoot)\Build//Debug//Vertex.so" -Destination "$($PSScriptRoot)/"
    write-host "copied to $("$($PSScriptRoot)/")"
    Copy-Item "$($PSScriptRoot)\Build//Debug//Pixel.so" -Destination "$($PSScriptRoot)/"
    write-host "copied to $("$($PSScriptRoot)/")"


}
  
end {}

 

