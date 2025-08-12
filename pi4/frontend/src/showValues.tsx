import React, { useEffect, useState } from 'react'
import axios from 'axios'

const GetImuData = () => {

    const [values, setValues] = useState<string[]>([])

    const fetchAllData = async () => {
        try {
        const response = await axios.get('http://192.168.12.109:8080/api/data')
        if (Array.isArray(response.data)) {
            setValues(response.data)
        } else {
            console.error('Unexpected data format', response.data)
        }
    } catch (error) {
        console.error('Failed to fetch', error)
    }
}
    
    useEffect(() => {
        fetchAllData()
    }, [])
    return (
        <div>
            <p>{values.join(', ')}</p>
        </div>
    )
}
export default GetImuData;