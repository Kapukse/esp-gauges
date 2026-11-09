let speed = Math.round($prop('SpeedKmh') || 0);
let rpm = Math.round($prop('Rpms') || 0);
let rpmPercent = Math.round(($prop('GameData.CarSettings_CurrentDisplayedRPMPercent') || 0) / 20);
let gear = $prop('Gear') || 'N';
let abs = $prop(('GameData.ABSActive') || 0);

return `${speed};${rpm};${rpmPercent};${gear};${abs}\n`;